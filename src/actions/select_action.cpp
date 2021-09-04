#include "select_action.h"

#include "../page_cache.h"
#include "../sql_parser/select_statement.h"
#include "../tuple.h"
#include "../sql_parser/expression_visitor.h"

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
    auto metadata_page = _page_cache.metadata_page_for_table(table_name);
    auto metadata = metadata::Metadata{metadata_page->column_names(), metadata_page->column_kinds()};

    std::vector<tuple::Tuple> tuples{};
    for (auto page: _page_cache.enumerate_pages(table_name, page::PageType::Data)) {
        auto data_page = dynamic_cast<page::SlottedDataPage *>(page);
        for (auto &&buffer: data_page->enumerate_tuples()) {
            tuple::Tuple tuple{buffer, metadata};
            if (matches(tuple, metadata)) {
                tuples.push_back(tuple);
            }
        }
    }
    return tuples;
}

bool actions::SelectAction::matches(const tuple::Tuple &t, const metadata::Metadata &m) const {
    if (!_where) {
        return true;
    }

    ast::EvaluatingExpressionVisitor v{t, m};
    auto result = v.evaluate(*_where);

    if (!result) {
        // TODO define exception here
        throw std::invalid_argument("failed to eval expression for tuple");
    }

    // A where clause must necessarily eval to a boolean value
    try {
        return std::get<bool>(*result);
    } catch (const std::bad_variant_access &) {
        // TODO define exception here
        throw std::invalid_argument("SELECT where clause not evaluating to boolean");
    }
}
