#include "page.h"
#include "streamutils.h"

#include <utility>
#include <sstream>

page::Page::Page(std::vector<unsigned char> buf) : _buffer{std::move(buf)},
                                                   _stream{stream_utils::build_binary_stream()} {
    _page_size = _buffer.size();
    stream_utils::load_stream_from_buffer(_stream, _buffer);
    read_header();
}

void page::Page::read_header() {
    _header_size = stream_utils::read_data_from_stream<decltype(_header_size)>(_stream);
    _page_id = stream_utils::read_data_from_stream<decltype(_page_id)>(_stream);
    _next_page_id = stream_utils::read_data_from_stream<decltype(_next_page_id)>(_stream);
    _prev_page_id = stream_utils::read_data_from_stream<decltype(_prev_page_id)>(_stream);
    _page_type = read_page_type();
    _free_space = stream_utils::read_data_from_stream<decltype(_free_space)>(_stream);
}


page::PageType page::Page::read_page_type() {
    auto page_type_as_int = stream_utils::read_data_from_stream<uint32_t>(_stream);
    switch (page_type_as_int) {
        case 0:
            return PageType::RowMap;
        case 1:
            return PageType::Data;
        case 2:
            return PageType::Metadata;
        default:
            throw std::invalid_argument("invalid page type: " + std::to_string(page_type_as_int));
    }
}

page::Page::Page() : _stream{stream_utils::build_binary_stream()} {}

page::Page::Page(uint32_t header_size, uint32_t page_id, uint32_t next_page_id, uint32_t prev_page_id,
                 page::PageType page_type, uint32_t page_size, uint32_t free_space)
    : _header_size(header_size),
      _page_id(page_id),
      _next_page_id(next_page_id),
      _prev_page_id(prev_page_id),
      _page_type(page_type),
      _page_size(page_size),
      _free_space(free_space),
      _stream{stream_utils::build_binary_stream()} {
}

void page::Page::write_header_to_stream() {
    _stream.seekp(0, std::ios::beg);
    stream_utils::write_data_to_stream(_stream, _header_size);
    stream_utils::write_data_to_stream(_stream, _page_id);
    stream_utils::write_data_to_stream(_stream, _next_page_id);
    stream_utils::write_data_to_stream(_stream, _prev_page_id);
    write_page_type();
    stream_utils::write_data_to_stream(_stream, _free_space);
}

void page::Page::write_page_type() {
    switch (_page_type) {
        case PageType::RowMap:
            stream_utils::write_data_to_stream<uint32_t>(_stream, 0);
            break;
        case PageType::Data:
            stream_utils::write_data_to_stream<uint32_t>(_stream, 1);
            break;
        case PageType::Metadata:
            stream_utils::write_data_to_stream<uint32_t>(_stream, 2);
            break;
    }
}

uint32_t page::Page::header_size() const {
    return _header_size;
}

uint32_t page::Page::page_id() const {
    return _page_id;
}

uint32_t page::Page::next_page_id() const {
    return _next_page_id;
}

uint32_t page::Page::prev_page_id() const {
    return _prev_page_id;
}

page::PageType page::Page::page_type() const {
    return _page_type;
}

uint32_t page::Page::page_size() const {
    return _page_size;
}

page::Page::Page(page::Page &&p) noexcept {
    _buffer = std::move(p._buffer);
    _stream = std::move(p._stream);
    _header_size = p._header_size;
    _page_id = p._page_id;
    _next_page_id = p._next_page_id;
    _prev_page_id = p._prev_page_id;
    _page_type = p._page_type;
    _page_size = p._page_size;
    _free_space = p._free_space;
}

page::Record page::Record::read_from_stream(std::stringstream &s) {
    auto row = stream_utils::read_data_from_stream<uint32_t>(s);
    auto page = stream_utils::read_data_from_stream<uint32_t>(s);
    auto slot = stream_utils::read_data_from_stream<uint32_t>(s);
    return Record(row, page, slot);
}

page::Record::Record(uint32_t row_id, uint32_t page_id, uint32_t slot_id) :
    row_id(row_id),
    page_id(page_id),
    slot_id(slot_id) {}

std::string page::Record::to_string() const {
    std::stringstream ss;
    ss << "Record{ row " << row_id << " page " << page_id << " slot " << slot_id << " }";
    return ss.str();
}

std::stringstream &page::Record::write_to_stream(std::stringstream &s) const {
    stream_utils::write_data_to_stream(s, row_id);
    stream_utils::write_data_to_stream(s, page_id);
    stream_utils::write_data_to_stream(s, slot_id);
    return s;
}

std::ostream &page::operator<<(std::ostream &os, page::PageType page_type) {
    switch (page_type) {
        case PageType::Data:
            os << "[data]";
            break;
        case PageType::RowMap:
            os << "[rowmap]";
            break;
        case PageType::Metadata:
            os << "[metadata]";
            break;
    }
    return os;
}
