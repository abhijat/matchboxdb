#include "metadata_page.h"
#include "metadata.h"
#include "streamutils.h"

#include <iostream>

/**
 * Creates a new page. This page is still in memory, it does not have a stream or a buffer attached yet.
 * Free space is page_size - header_size - data_size.
 */
page::MetadataPage::MetadataPage(std::string table_name, std::vector<std::string> column_names,
                                 std::vector<metadata::Kind> column_kinds, uint32_t n_data_pages,
                                 uint32_t n_rowmap_pages, uint32_t max_row_id, uint32_t total_pages) :
    _table_name(std::move(table_name)),
    _column_names(std::move(column_names)),
    _column_kinds(std::move(column_kinds)),
    _n_data_pages{n_data_pages},
    _n_rowmap_pages{n_rowmap_pages},
    _max_row_id{max_row_id},
    _n_total_pages{total_pages},
    _n_marked_pages{0},
    _n_unmarked_pages{total_pages},
    Page{
        0,
        0,
        0,
        PageType::Metadata,
        k_page_size,
        k_page_size,
    } {

    auto data_size = stream_utils::size_of_string(_table_name);
    data_size += stream_utils::size_of_strings(_column_names);
    data_size += stream_utils::size_of_kinds(column_kinds);
    data_size += sizeof(_n_data_pages);
    data_size += sizeof(_n_rowmap_pages);
    data_size += sizeof(_max_row_id);
    data_size += sizeof(_n_total_pages);
    data_size += sizeof(_n_marked_pages);
    data_size += sizeof(_n_unmarked_pages);
    _free_space = k_page_size - (data_size + _base_header_size);
}

page::MetadataPage::MetadataPage(const stream_utils::ByteBuffer &buffer) : Page{buffer} {
    _table_name = stream_utils::read_string_from_stream(_stream);
    _column_names = stream_utils::read_strings_from_stream(_stream);
    _column_kinds = stream_utils::read_kinds_from_stream(_stream);
    _n_data_pages = stream_utils::read_data_from_stream<decltype(_n_data_pages)>(_stream);
    _n_rowmap_pages = stream_utils::read_data_from_stream<decltype(_n_rowmap_pages)>(_stream);
    _max_row_id = stream_utils::read_data_from_stream<decltype(_max_row_id)>(_stream);
    _n_total_pages = stream_utils::read_data_from_stream<decltype(_n_total_pages)>(_stream);
    _n_marked_pages = stream_utils::read_data_from_stream<decltype(_n_marked_pages)>(_stream);
    _n_unmarked_pages = stream_utils::read_data_from_stream<decltype(_n_unmarked_pages)>(_stream);
}

std::string page::MetadataPage::to_string() const {
    std::stringstream ss;
    ss << "table name: " << _table_name;

    ss << "\n";
    for (const auto &column: _column_names) {
        ss << " {" << column << "} ";
    }

    ss << "\n";
    for (const auto &kind: _column_kinds) {
        switch (kind) {
            case metadata::Kind::String:
                ss << " [string] ";
                break;
            case metadata::Kind::UnsignedInt:
                ss << " [uint] ";
                break;
            case metadata::Kind::Boolean:
                ss << " [bool] ";
                break;
        }
    }

    ss << "\n";
    ss << " _n_data_pages " << _n_data_pages
       << " _n_rowmap_pages " << _n_rowmap_pages
       << " _max_row_id " << _max_row_id
       << " _n_total_pages " << _n_total_pages
       << " _n_marked_pages " << _n_marked_pages
       << " _n_unmarked_pages " << _n_unmarked_pages;

    return ss.str();
}

/**
 * Converts a page to buffer, with the free space filled in with null bytes.
 */
stream_utils::ByteBuffer page::MetadataPage::buffer() {
    write_header_to_stream();
    stream_utils::write_string_to_stream(_stream, _table_name);
    stream_utils::write_strings_to_stream(_stream, _column_names);
    stream_utils::write_kinds_to_stream(_stream, _column_kinds);
    stream_utils::write_data_to_stream(_stream, _n_data_pages);
    stream_utils::write_data_to_stream(_stream, _n_rowmap_pages);
    stream_utils::write_data_to_stream(_stream, _max_row_id);
    stream_utils::write_data_to_stream(_stream, _n_total_pages);
    stream_utils::write_data_to_stream(_stream, _n_marked_pages);
    stream_utils::write_data_to_stream(_stream, _n_unmarked_pages);
    auto buffer = stream_utils::buffer_from_stream(_stream);
    auto buffer_size = buffer.size();

    buffer.insert(buffer.end(), page::k_page_size - buffer_size, '\0');
    return buffer;
}

uint32_t page::MetadataPage::free_space() const {
    return _free_space;
}

uint32_t page::MetadataPage::n_data_pages() const {
    return _n_data_pages;
}

uint32_t page::MetadataPage::n_rowmap_pages() const {
    return _n_rowmap_pages;
}

const std::string &page::MetadataPage::table_name() const {
    return _table_name;
}

const std::vector<std::string> &page::MetadataPage::column_names() const {
    return _column_names;
}

const std::vector<metadata::Kind> &page::MetadataPage::column_kinds() const {
    return _column_kinds;
}

page::MetadataPage::MetadataPage(page::MetadataPage &&m) noexcept: Page(std::move(m)) {
    _n_data_pages = m._n_data_pages;
    _n_rowmap_pages = m._n_rowmap_pages;
    _table_name = std::move(m._table_name);
    _column_names = std::move(m._column_names);
    _column_kinds = std::move(m._column_kinds);
    _max_row_id = m._max_row_id;
    _n_total_pages = m._n_total_pages;
    _n_marked_pages = m._n_marked_pages;
    _n_unmarked_pages = m._n_unmarked_pages;
    _free_space = m._free_space;
}

uint32_t page::MetadataPage::next_row_id() {
    _max_row_id += 1;
    return _max_row_id;
}

uint32_t page::MetadataPage::max_row_id() const {
    return _max_row_id;
}

stream_utils::ByteBuffer page::MetadataPage::empty_page() {
    return stream_utils::ByteBuffer();
}

uint32_t page::MetadataPage::n_total_pages() const {
    return _n_total_pages;
}

uint32_t page::MetadataPage::n_marked_pages() const {
    return _n_marked_pages;
}

uint32_t page::MetadataPage::n_unmarked_pages() const {
    return _n_unmarked_pages;
}
