#include <fstream>
#include <utility>
#include "page_creator.h"
#include "page_cache.h"

page::PageCreator::PageCreator(std::string table_name, page::MetadataPage *metadata_page) :
    _table_name(std::move(table_name)), _metadata_page(metadata_page) {}

page::PageId page::PageCreator::create_page(page::PageType page_type) {
    if (!_metadata_page->has_pages_available()) {
        std::stringstream err;
        err << "table space is full for " << _table_name;
        throw std::out_of_range{err.str()};
    }

    auto file_name = page_cache::file_name_from_table_name(_table_name);
    auto f = stream_utils::open_db_file(file_name);

    // add one extra page to account for the metadata page.
    // TODO should we start n_marked_pages as 1?
    auto offset = page::k_page_size * (_metadata_page->n_marked_pages() + 1);

    f.seekp(offset, std::ios::beg);

    switch (page_type) {
        case PageType::Data:
            return create_data_page(f);
        case PageType::RowMap:
            return create_row_map_page(f);
        default:
            throw std::invalid_argument{"Unsupported page type"};
    }
}

page::PageId page::PageCreator::create_data_page(std::fstream &f) {
    auto page_id = _metadata_page->next_page_id_for_table();
    page::SlottedDataPage slotted_data_page{page_id, page::k_page_size};
    stream_utils::write_page_to_stream(f, slotted_data_page.empty_page());
    _metadata_page->increment_marked_pages(PageType::Data);
    return page_id;
}

page::PageId page::PageCreator::create_row_map_page(std::fstream &f) {
    auto page_id = _metadata_page->next_page_id_for_table();
    page::RowMappingPage row_mapping_page{page_id, page::k_page_size};
    stream_utils::write_page_to_stream(f, row_mapping_page.empty_page());
    _metadata_page->increment_marked_pages(PageType::RowMap);
    return page_id;
}
