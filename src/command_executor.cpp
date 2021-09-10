#include <iostream>
#include "command_executor.h"

#include "insert_object.h"
#include "page/page_cache.h"
#include "sql_parser/create_statement.h"
#include "sql_parser/delete_statement.h"
#include "sql_parser/insert_statement.h"
#include "sql_parser/update_statement.h"
#include "sql_parser/select_statement.h"
#include "actions/select_action.h"
#include "actions/delete_action.h"
#include "actions/update_action.h"

command_executor::CommandExecutor::CommandExecutor(page_cache::PageCache &page_cache)
    : _page_cache(page_cache) {
}

void command_executor::CommandExecutor::visit(const ast::InsertStatement &insert_statement) {
    std::vector<metadata::DataType> attributes(insert_statement.inserts().size());

    std::cout << "executing insert statement: " << insert_statement << "\n";

    std::transform(
        std::cbegin(insert_statement.inserts()),
        std::cend(insert_statement.inserts()),
        std::begin(attributes),
        [](const auto &expression) {
            auto evaluated = expression->evaluate();
            if (!evaluated) {
                std::stringstream ss;
                ss << "failed to convert input to a usable value: [" << *expression << "]";
                throw std::invalid_argument(ss.str());
            }
            return *evaluated;
        });

    actions::InsertObject insert_object{
        _page_cache,
        insert_statement.table().table_name(),
        tuple::Tuple{attributes}
    };
    _command_execution_result = insert_object.save();
}

void command_executor::CommandExecutor::visit(const ast::DeleteStatement &delete_statement) {
    std::cout << "executing delete statement: " << delete_statement << "\n";
    actions::DeleteAction delete_action{_page_cache, delete_statement};
    _command_execution_result = delete_action.apply_delete();
}

void command_executor::CommandExecutor::visit(const ast::CreateStatement &create_statement) {}

void command_executor::CommandExecutor::visit(const ast::UpdateStatement &update_statement) {
    std::cout << "executing update statement: " << update_statement << "\n";
    actions::UpdateAction update_action{_page_cache, update_statement};
    _command_execution_result = update_action.update();
}

command_executor::CommandExecutionResult command_executor::CommandExecutor::execute(const ast::Statement &statement) {
    statement.accept(*this);
    return _command_execution_result;
}

void command_executor::CommandExecutor::visit(const ast::SelectStatement &select_statement) {
    std::cout << "executing select statement: " << select_statement << "\n";
    actions::SelectAction select_action{_page_cache, select_statement};
    _command_execution_result = select_action.list();
}
