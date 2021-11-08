#include <iostream>
#include "slotted_data_page.h"
#include "../logging.h"

page::SlottedDataPage::SlottedDataPage(const std::vector<unsigned char> &buffer) : Page(buffer) {
    // since the parent constructor has run, we should be at the right offset in the stream to read our fields.
    _slot_end_marker = stream_utils::read_data_from_stream<decltype(_slot_end_marker)>(_stream);
    _tuple_begin_marker = stream_utils::read_data_from_stream<decltype(_tuple_begin_marker)>(_stream);
}

uint32_t page::SlottedDataPage::store_tuple(const tuple::Tuple &tuple) {

    std::lock_guard<std::mutex> guard{_mutex};

    log::info("storing tuple", tuple);

    uint32_t effective_tuple_width = tuple.size() + sizeof(uint32_t);

    uint32_t space_required = effective_tuple_width + sizeof(uint32_t);

    if (_free_space < space_required) {
        std::stringstream err;
        err << "not enough free space, required: (" << space_required << "), have (" << _free_space << ")";
        throw std::out_of_range(err.str());
    }

    uint32_t write_begins_at = _tuple_begin_marker - effective_tuple_width;
    _stream.seekp(write_begins_at, std::ios::beg);

    stream_utils::write_byte_buffer_to_stream(_stream, tuple.data());

    _stream.seekp(_slot_end_marker, std::ios::beg);
    stream_utils::write_data_to_stream(_stream, write_begins_at);
    _slot_end_marker = _stream.tellp();

    _tuple_begin_marker = write_begins_at;

    _free_space = _tuple_begin_marker - _slot_end_marker;

    auto total_slot_size = _slot_end_marker - header_size();
    auto total_slots = total_slot_size / sizeof(uint32_t);

    return total_slots - 1;
}

std::string page::SlottedDataPage::to_string() const {
    std::stringstream ss;
    ss << "page id " << _page_id
       << " header size " << header_size()
       << " next page " << _next_page_id
       << " previous page " << _prev_page_id
       << " slot end " << _slot_end_marker
       << " tuple begin " << _tuple_begin_marker
       << " free space " << _free_space
       << " page size " << _page_size;

    switch (_page_type) {
        case page::PageType::Data:
            ss << " page type: data ";
            break;
        case PageType::Metadata:
            ss << " page type: metadata ";
            break;
    }

    return ss.str();

}

std::vector<unsigned char> page::SlottedDataPage::read_tuple_from_slot(uint32_t slot_id) {
    // seek to the slot id
    uint32_t offset = header_size() + (slot_id * sizeof(uint32_t));
    _stream.seekg(offset, std::ios::beg);

    // read the tuple offset
    uint32_t tuple_offset;
    _stream.read(reinterpret_cast<char *>(&tuple_offset), sizeof(tuple_offset));

    // seek to the tuple offset
    _stream.seekg(tuple_offset, std::ios::beg);

    // read the tuple size
    uint32_t tuple_size;
    _stream.read(reinterpret_cast<char *>(&tuple_size), sizeof(tuple_size));

    // read tuple_size bytes
    char buf[tuple_size];
    _stream.read(buf, tuple_size);
    if (!_stream) {
        std::cout << "failed for slot " << slot_id << "\n";
        std::cout << "_stream.gcount() " << _stream.gcount() << "\n";
    }

    return std::vector<unsigned char>{buf, buf + tuple_size};
}

page::SlottedDataPage::SlottedDataPage(uint32_t page_id, uint32_t page_size)
    : Page(page_id, 0, 0, PageType::Data, page_size, page_size - header_size()) {
    _slot_end_marker = header_size();
    _tuple_begin_marker = page_size;
}

/**
 * Converts slotted data page to buffer. First writes all the updated header values to stream, then returns the
 * underlying buffer. Note that data other than the header/metadata fields is already written in the right locations
 * inside _stream. We only need to sync the metadata etc.
 */
stream_utils::ByteBuffer page::SlottedDataPage::buffer() {
    write_header_to_stream();
    stream_utils::write_data_to_stream(_stream, _slot_end_marker);
    stream_utils::write_data_to_stream(_stream, _tuple_begin_marker);
    return stream_utils::buffer_from_stream(_stream);
}

uint32_t page::SlottedDataPage::free_space() const {
    return _free_space;
}

stream_utils::ByteBuffer page::SlottedDataPage::empty_page() {
    write_header_to_stream();
    stream_utils::write_data_to_stream(_stream, _slot_end_marker);
    stream_utils::write_data_to_stream(_stream, _tuple_begin_marker);
    uint32_t data_written = _stream.tellp();
    auto buffer = stream_utils::buffer_from_stream(_stream);
    buffer.insert(buffer.end(), k_page_size - data_written, '\0');
    return buffer;
}

uint32_t page::SlottedDataPage::slot_end_marker() const {
    return _slot_end_marker;
}

uint32_t page::SlottedDataPage::tuple_begin_marker() const {
    return _tuple_begin_marker;
}

std::vector<page::TupleWithSlotId> page::SlottedDataPage::enumerate_tuples() {
    log::info("enumerating tuples...");

    std::vector<uint32_t> offsets{};
    std::vector<uint32_t> slot_ids{};

    _stream.seekg(header_size(), std::ios::beg);

    // store all the tuple locations first
    auto slot_id = 0;
    while (_stream.tellg() != _slot_end_marker) {
        auto offset = stream_utils::read_data_from_stream<uint32_t>(_stream);
        if (offset != 0) {
            // TODO use a better check here for tombstones
            offsets.push_back(offset);

            slot_ids.push_back(slot_id);
        } else {
            log::info("skipping deleted slot id", slot_id);
        }

        slot_id += 1;
    }

    // reverse the offsets, so we do not have to jump backwards when seeking
    std::reverse(offsets.begin(), offsets.end());

    auto tuple_buffers = load_tuple_buffers(offsets);

    // reverse the tuples, so that the slot ids match the tuples
    std::reverse(tuple_buffers.begin(), tuple_buffers.end());

    std::vector<page::TupleWithSlotId> tuple_with_slot_id(slot_ids.size());

    std::transform(std::cbegin(slot_ids), std::cend(slot_ids), std::cbegin(tuple_buffers),
                   std::begin(tuple_with_slot_id),
                   [](auto slot, auto buffer) {
                       return page::TupleWithSlotId{slot, buffer};
                   });

    return tuple_with_slot_id;
}

void page::SlottedDataPage::delete_tuple_at_slot_id(uint32_t slot_id) {
    std::lock_guard<std::mutex> guard{_mutex};

    uint32_t offset = header_size() + (slot_id * sizeof(uint32_t));
    _stream.seekp(offset, std::ios::beg);

    auto current_position = _stream.tellp();
    if (!_stream || _stream.tellp() >= _slot_end_marker) {
        throw std::out_of_range{"deleting slot id will overwrite data in page: " + std::to_string(slot_id)};
    }

    // TODO write the real tombstone value here
    log::info("marked tuple at slot id", slot_id, "as deleted");

    _stream.seekg(offset, std::ios::beg);
    auto tuple_offset = stream_utils::read_data_from_stream<uint32_t>(_stream);
    if (tuple_offset == 0) {
        log::info("WARNING: the slot is already deleted", slot_id);
    } else {
        stream_utils::write_data_to_stream<uint32_t>(_stream, 0);
    }
}

uint32_t page::SlottedDataPage::defrag_page(const metadata::Metadata &metadata) {
    _stream.seekg(header_size(), std::ios::beg);

    // pick out the offsets where non-deleted tuples are stored. deleted offsets are skipped, and will be overwritten
    std::vector<uint32_t> offsets{};
    while (_stream.tellg() != _slot_end_marker) {
        auto offset = stream_utils::read_data_from_stream<uint32_t>(_stream);
        if (offset != 0) {
            offsets.push_back(offset);
        }
    }

    auto tuple_buffers = load_tuple_buffers(offsets);

    // store the original free space for saved space calculation,
    // then reset the page so that appears empty to store_tuple.
    uint32_t original_free_space = free_space();
    reset_markers();

    // Use the store_tuple api to start storing non deleted tuples from the beginning.
    // store_tuple should take care of updating internal page state.
    for (const auto &tuple_buffer: tuple_buffers) {
        store_tuple(tuple::Tuple{tuple_buffer, metadata});
    }

    return free_space() - original_free_space;
}

void page::SlottedDataPage::reset_markers() {
    _slot_end_marker = header_size();
    _tuple_begin_marker = _page_size;
    _free_space = _tuple_begin_marker - _slot_end_marker;
}

std::vector<stream_utils::ByteBuffer> page::SlottedDataPage::load_tuple_buffers(const std::vector<uint32_t> &offsets) {
    std::vector<stream_utils::ByteBuffer> tuple_buffers{offsets.size()};
    std::for_each(offsets.cbegin(),  offsets.cend(), [](auto offset) {
        log::info("offset: ", offset);
    });
    std::transform(offsets.cbegin(), offsets.cend(), tuple_buffers.begin(),
                   [&](const auto &offset) { return load_tuple_buffer(offset); });
    return tuple_buffers;
}

stream_utils::ByteBuffer page::SlottedDataPage::load_tuple_buffer(uint32_t offset) {
    _stream.seekg(offset, std::ios::beg);
    auto tuple_size = stream_utils::read_data_from_stream<uint32_t>(_stream);

    char buf[tuple_size];
    _stream.read(buf, tuple_size);
    if (!_stream) {
        log::info("failed to read stream when trying to read tuple at offset", offset);
        throw std::ios::failure{"failed to read stream when trying to read tuple at offset " + std::to_string(offset)};
    }

    return std::vector<unsigned char>{buf, buf + tuple_size};
}

page::SlottedDataPage::SlottedDataPage(const page::SlottedDataPage &slotted_data_page)
    : Page(
    slotted_data_page.page_id(),
    slotted_data_page.next_page_id(),
    slotted_data_page.prev_page_id(),
    PageType::Data,
    slotted_data_page.page_size(),
    slotted_data_page.free_space()
) {
    _slot_end_marker = slotted_data_page.slot_end_marker();
    _tuple_begin_marker = slotted_data_page.tuple_begin_marker();
    _free_space = slotted_data_page.free_space();
    _stream << slotted_data_page._stream.rdbuf();
    _buffer = slotted_data_page._buffer;
    _version = slotted_data_page._version;
}

void page::SlottedDataPage::overwrite_with(const page::SlottedDataPage &slotted_data_page) {
    _slot_end_marker = slotted_data_page.slot_end_marker();
    _tuple_begin_marker = slotted_data_page.tuple_begin_marker();
    _free_space = slotted_data_page.free_space();
    _stream << slotted_data_page._stream.rdbuf();
    _buffer = slotted_data_page._buffer;
    _version = slotted_data_page._version;
}
