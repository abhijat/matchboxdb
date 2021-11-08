#include "delete_action.h"

#include "../sql_parser/delete_statement.h"
#include "../page/page_cache.h"
#include "../sql_parser/expression.h"
#include "../tuple_filter.h"
#include "../logging.h"

actions::DeleteAction::DeleteAction(page_cache::PageCache &page_cache, const ast::DeleteStatement &delete_statement)
    : _page_cache(page_cache), _delete_statement(delete_statement) {
    if (_delete_statement.where()) {
        _where = _delete_statement.where()->get();
    } else {
        _where = nullptr;
    }
}

uint32_t actions::DeleteAction::apply_delete() {
    auto table_name = _delete_statement.table().table_name();
    auto metadata = _page_cache.metadata_for_table(table_name);

    uint32_t deleted_count{};
    for (auto page: _page_cache.enumerate_pages(table_name, page::PageType::Data)) {
        auto data_page = dynamic_cast<page::SlottedDataPage *>(page);
        for (auto &&tuple_with_slot: data_page->enumerate_tuples()) {
            auto tuple = tuple::Tuple{tuple_with_slot.byte_buffer, metadata};
            if (tuple_filter::matches(tuple, metadata, _where)) {
                log::info("deleting matched row: ", tuple);
                data_page->delete_tuple_at_slot_id(tuple_with_slot.slot_id);
                _page_cache.mark_page_dirty(table_name, data_page, true);
                deleted_count += 1;
            }
        }
    }

    return deleted_count;
}
