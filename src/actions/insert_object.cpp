#include "insert_object.h"

#include "../page/page_cache.h"

#include <utility>

actions::InsertObject::InsertObject(page_cache::PageCache &page_cache, std::string table_name, tuple::Tuple tuple)
    : _page_cache(page_cache), _table_name(std::move(table_name)), _tuple(std::move(tuple)) {
}

uint32_t actions::InsertObject::save() {
    uint32_t size_required_for_tuple = _tuple.size() + (2 * sizeof(uint32_t));
    auto data_page = _page_cache.get_page_for_data_size(_table_name, size_required_for_tuple);

    auto slot_id = data_page->store_tuple(_tuple);
    return _page_cache.next_row_id_for_table(_table_name);
}
