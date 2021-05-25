#include "select_object.h"

actions::SelectObject::SelectObject(page_cache::PageCache &page_cache, const std::string &table_name, uint32_t row_id)
    : _page_cache(page_cache), _table_name(table_name), _row_id(row_id) {}

tuple::Tuple actions::SelectObject::get() {
    auto rowmap_page = _page_cache.rowmap_page_for_row_id(_table_name, _row_id);
    auto record = rowmap_page->record_for_row_id(_row_id);
    auto data_page = dynamic_cast<page::SlottedDataPage *>(_page_cache.get_page_id(record.page_id,
                                                                                   _table_name,
                                                                                   page::PageType::Data));
    auto tuple_as_buffer = data_page->read_tuple_from_slot(record.slot_id);
    auto metadata_page = _page_cache.metadata_page_for_table(_table_name);
    metadata::Metadata m{metadata_page->column_names(), metadata_page->column_kinds()};
    return {tuple_as_buffer, m};
}
