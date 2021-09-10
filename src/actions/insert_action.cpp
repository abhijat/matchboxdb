#include "insert_action.h"
#include "../page/page_cache.h"
#include "../sql_parser/expression_visitor.h"
#include "../sql_parser/expression.h"
#include "../sql_parser/insert_statement.h"
#include "../insert_object.h"

actions::InsertAction::InsertAction(page_cache::PageCache &page_cache, const ast::InsertStatement &insert_statement)
    : _page_cache(page_cache), _insert_statement(insert_statement) {
}

uint32_t actions::InsertAction::save() {
    auto table_name = _insert_statement.table().table_name();
    tuple::Tuple t{build_attributes_for_tuple()};
    auto[row_map_page, data_page] = _page_cache.get_pages_for_data_size(table_name,
                                                                        t.storage_size_required_for_tuple());

    auto slot_id = data_page->store_tuple(t);
    auto row_id = _page_cache.next_row_id_for_table(table_name);

    row_map_page->store_record({row_id, data_page->page_id(), slot_id});
    return row_id;
}

std::vector<metadata::DataType> actions::InsertAction::build_attributes_for_tuple() const {
    auto evaluator = ast::EvaluatingExpressionVisitor::context_less_evaluating_expression_visitor();
    std::vector<metadata::DataType> attributes(_insert_statement.inserts().size());

    std::transform(
        std::cbegin(_insert_statement.inserts()),
        std::cend(_insert_statement.inserts()),
        std::begin(attributes),
        [&](const auto &expression) {
            auto evaluated = evaluator.evaluate(*expression);
            if (!evaluated) {
                std::stringstream ss;
                ss << "failed to convert input to a usable value: [" << *expression << "]";
                throw std::invalid_argument(ss.str());
            }
            return *evaluated;
        });

    return attributes;
}
