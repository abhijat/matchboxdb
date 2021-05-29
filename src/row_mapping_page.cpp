#include "row_mapping_page.h"

std::vector<page::Record> page::RowMappingPage::enumerate_records() {
    if (_page_type != PageType::RowMap) {
        throw std::invalid_argument("Page is not rowmap");
    }

    _stream.seekg(header_size(), std::ios::beg);

    std::vector<Record> records;
    auto records_read = 0;
    while (records_read <= _n_records) {
        records.push_back(Record::read_from_stream(_stream));
        records_read += 1;
    }

    return records;
}

page::RowMappingPage::RowMappingPage(const std::vector<unsigned char> &buffer) : Page(buffer) {
    _n_records = stream_utils::read_data_from_stream<decltype(_n_records)>(_stream);
    _max_row_id = stream_utils::read_data_from_stream<decltype(_max_row_id)>(_stream);
}

std::string page::RowMappingPage::to_string() const {
    std::stringstream ss;
    ss << "page id " << _page_id << " header size " << _base_header_size << " next page " << _next_page_id
       << " previous page "
       << _prev_page_id;

    ss << " max row id " << _max_row_id << " num records " << _n_records << " free space " << _free_space
       << " page size "
       << _page_size;

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

uint32_t page::RowMappingPage::free_space() const {
    return _free_space;
}

stream_utils::ByteBuffer page::RowMappingPage::buffer() {
    write_header_to_stream();
    stream_utils::write_data_to_stream(_stream, _n_records);
    stream_utils::write_data_to_stream(_stream, _max_row_id);
    return stream_utils::buffer_from_stream(_stream);
}

stream_utils::ByteBuffer page::RowMappingPage::empty_page() {
    write_header_to_stream();
    stream_utils::write_data_to_stream(_stream, _n_records);
    stream_utils::write_data_to_stream(_stream, _max_row_id);
    auto buffer = stream_utils::buffer_from_stream(_stream);
    buffer.insert(buffer.end(), _free_space, '\0');
    return buffer;
}

page::RowMappingPage::RowMappingPage(uint32_t page_id, uint32_t page_size) : Page(
    page_id,
    0,
    0,
    PageType::RowMap,
    page_size,
    page_size - header_size()) {
}

void page::RowMappingPage::store_record(const page::Record &record) {
    uint32_t offset = header_size() + record_size();
    _stream.seekp(offset, std::ios::beg);

    record.write_to_stream(_stream);

    _n_records += 1;
    _free_space = k_page_size - (header_size() + record_size());
}

page::Record page::RowMappingPage::record_for_row_id(uint32_t row_id) {
    _stream.seekg(header_size(), std::ios::beg);
    auto records_read = 0;
    while (records_read <= _n_records) {
        auto record = Record::read_from_stream(_stream);
        if (record.row_id == row_id) {
            return record;
        }

        records_read += 1;
    }

    throw std::out_of_range{"record not found for row id " + std::to_string(row_id)};
}

uint32_t page::RowMappingPage::record_size() const {
    return _n_records * k_record_width;
}
