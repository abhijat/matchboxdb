#include <iostream>
#include "slotted_data_page.h"
#include "streamutils.h"

page::SlottedDataPage::SlottedDataPage(const std::vector<unsigned char> &buffer) : Page(buffer) {
    _stream.seekg(sizeof(uint32_t) * 6, std::ios::beg);
    _slot_end_marker = stream_utils::read_data_from_stream<decltype(_slot_end_marker)>(_stream);
    _tuple_begin_marker = stream_utils::read_data_from_stream<decltype(_tuple_begin_marker)>(_stream);
}

uint32_t page::SlottedDataPage::store_tuple(const tuple::Tuple &tuple) {
    uint32_t effective_tuple_width = tuple.size() + sizeof(uint32_t);

    uint32_t write_begins_at = _tuple_begin_marker - effective_tuple_width;
    _stream.seekp(write_begins_at, std::ios::beg);

    stream_utils::write_byte_buffer_to_stream(_stream, tuple.data());

    _stream.seekp(_slot_end_marker, std::ios::beg);
    stream_utils::write_data_to_stream(_stream, write_begins_at);
    _slot_end_marker = _stream.tellp();

    _tuple_begin_marker = write_begins_at;

    _free_space = _tuple_begin_marker - _slot_end_marker;

    auto total_slot_size = _slot_end_marker - (_header_size + 2 * sizeof(uint32_t));
    auto total_slots = total_slot_size / sizeof(uint32_t);

    return total_slots - 1;
}

std::string page::SlottedDataPage::to_string() const {
    std::stringstream ss;
    ss << "page id " << _page_id
       << " header size " << _header_size
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
        case page::PageType::RowMap:
            ss << " page type: row map ";
            break;
        case PageType::Metadata:
            ss << " page type: metadata ";
            break;
    }

    return ss.str();

}

std::vector<unsigned char> page::SlottedDataPage::read_tuple_from_slot(uint32_t slot_id) {
    // seek to the slot id
    uint32_t offset = _header_size + (2 * sizeof(uint32_t)) + (slot_id * sizeof(uint32_t));
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

page::SlottedDataPage::SlottedDataPage(uint32_t header_size, uint32_t page_id, uint32_t page_size)
    : Page(header_size, page_id, 0, 0, PageType::Data, page_size, page_size - header_size) {
    _slot_end_marker = header_size + sizeof(_slot_end_marker) + sizeof(_tuple_begin_marker);
    _tuple_begin_marker = page_size;
    _buffer = stream_utils::ByteBuffer(_free_space);
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
    _free_space = k_page_size - data_written;

    auto buffer = stream_utils::buffer_from_stream(_stream);
    buffer.insert(buffer.end(), _free_space, '\0');
    return buffer;
}
