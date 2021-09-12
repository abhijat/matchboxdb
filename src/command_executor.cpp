#include <iostream>
#include <filesystem>
#include "command_executor.h"

#include "actions/delete_action.h"
#include "actions/insert_action.h"
#include "actions/select_action.h"
#include "actions/update_action.h"
#include "page/page_cache.h"
#include "sql_parser/create_statement.h"
#include "sql_parser/delete_statement.h"
#include "sql_parser/drop_statement.h"
#include "sql_parser/insert_statement.h"
#include "sql_parser/select_statement.h"
#include "sql_parser/update_statement.h"
#include "storage/table_initializer.h"
#include "storage/utils.h"
#include "logging.h"

command_executor::CommandExecutor::CommandExecutor(page_cache::PageCache &page_cache)
    : _page_cache(page_cache) {
}

void command_executor::CommandExecutor::visit(const ast::InsertStatement &insert_statement) {
    log::info("executing insert statement:", insert_statement);
    actions::InsertAction insert_action{_page_cache, insert_statement};
    _command_execution_result = insert_action.save();
    _page_cache.write_dirty_pages(insert_statement.table().table_name());
}

void command_executor::CommandExecutor::visit(const ast::DeleteStatement &delete_statement) {
    log::info("executing delete statement:", delete_statement);
    actions::DeleteAction delete_action{_page_cache, delete_statement};
    _command_execution_result = delete_action.apply_delete();
    _page_cache.write_dirty_pages(delete_statement.table().table_name());
}

void command_executor::CommandExecutor::visit(const ast::CreateStatement &create_statement) {
    log::info("executing create table statement:", create_statement);
    initializers::TableInitializer table_initializer{create_statement, initializers::k_table_size_in_mb};
    table_initializer.initialize();
    _page_cache.add_new_table(create_statement.table_name().table_name());
    _command_execution_result = 0;
}

void command_executor::CommandExecutor::visit(const ast::UpdateStatement &update_statement) {
    log::info("executing update statement:", update_statement);
    actions::UpdateAction update_action{_page_cache, update_statement};
    _command_execution_result = update_action.update();
    _page_cache.write_dirty_pages(update_statement.table().table_name());
}

command_executor::CommandExecutionResult command_executor::CommandExecutor::execute(const ast::Statement &statement) {
    statement.accept(*this);
    return _command_execution_result;
}

void command_executor::CommandExecutor::visit(const ast::SelectStatement &select_statement) {
    log::info("executing select statement: ", select_statement);
    actions::SelectAction select_action{_page_cache, select_statement};
    _command_execution_result = select_action.list();
}

void command_executor::CommandExecutor::visit(const ast::DropStatement &drop_statement) {
    log::info("executing drop statement: ", drop_statement);
    auto file_name = storage_utils::file_name_from_table_name(drop_statement.table_name());
    if (std::filesystem::exists(file_name)) {
        std::filesystem::remove(file_name);
    } else {
        std::cout << "Table does not exist: " << drop_statement.table_name();
    }
    _command_execution_result = 0;
}
