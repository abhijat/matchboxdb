#include <iostream>
#include "command_executor.h"

#include "insert_object.h"
#include "page_cache.h"
#include "sql_parser/create_statement.h"
#include "sql_parser/delete_statement.h"
#include "sql_parser/insert_statement.h"
#include "sql_parser/update_statement.h"
#include "sql_parser/select_statement.h"

command_executor::CommandExecutor::CommandExecutor(page_cache::PageCache &page_cache)
    : _page_cache(page_cache) {
}

uint32_t command_executor::CommandExecutor::visit(const ast::InsertStatement &insert_statement) {
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
    return insert_object.save();
}

void command_executor::CommandExecutor::visit(const ast::DeleteStatement &delete_statement) {}

void command_executor::CommandExecutor::visit(const ast::CreateStatement &create_statement) {}

void command_executor::CommandExecutor::visit(const ast::UpdateStatement &update_statement) {}

void command_executor::CommandExecutor::execute(const ast::Statement &statement) {
    statement.accept(*this);
}

void command_executor::CommandExecutor::visit(const ast::SelectStatement &select_statement) {
    std::cout << "executing select statement: " << select_statement << "\n";
}
