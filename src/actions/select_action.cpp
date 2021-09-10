#include "select_action.h"

#include "../page/page_cache.h"
#include "../sql_parser/select_statement.h"
#include "../tuple.h"
#include "../sql_parser/expression_visitor.h"
#include "../tuple_filter.h"

actions::SelectAction::SelectAction(page_cache::PageCache &page_cache, const ast::SelectStatement &select_statement) :
    _page_cache(page_cache),
    _select_statement(select_statement) {
    if (_select_statement.where()) {
        _where = select_statement.where()->get();
    } else {
        _where = nullptr;
    }
}

std::vector<tuple::Tuple> actions::SelectAction::list() {
    auto table_name = _select_statement.table()->table_name();
    auto metadata = _page_cache.metadata_for_table(table_name);

    std::vector<tuple::Tuple> tuples{};
    for (auto page: _page_cache.enumerate_pages(table_name, page::PageType::Data)) {
        auto data_page = dynamic_cast<page::SlottedDataPage *>(page);
        for (auto &&tuple_with_slot_id: data_page->enumerate_tuples()) {
            tuple::Tuple tuple{tuple_with_slot_id.byte_buffer, metadata};
            if (tuple_filter::matches(tuple, metadata, _where)) {
                tuples.push_back(tuple);
            }
        }
    }
    return tuples;
}
