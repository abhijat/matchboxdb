#include "test_utilities.h"

#include <filesystem>

#include "../src/sql_parser/parser.h"
#include "../src/storage/table_initializer.h"
#include "../src/storage/utils.h"

void testutils::create_test_table() {
    create_test_table(k_table_name, {{"name", token::Kind::ColumnKindString},
                                     {"age",  token::Kind::ColumnKindUnsignedInteger}}, k_table_size_mb);
}

void testutils::cleanup_test_table() {
    cleanup_test_table(k_table_name);
}

void testutils::TestsWithRealTable::SetUp() {
    create_test_table();
}

void testutils::TestsWithRealTable::TearDown() {
    cleanup_test_table();
}

std::unique_ptr<ast::Statement> testutils::parse(const std::string &s) {
    return parser::Parser{lexer::Lexer{s}}.parse();
}

command_executor::CommandExecutionResult testutils::execute(command_executor::CommandExecutor &executor,
                                                            const std::string &statement) {
    auto parsed = parser::Parser{lexer::Lexer{statement}}.parse();
    return executor.execute(*parsed);
}

void
testutils::create_test_table(const std::string &table_name, const std::vector<ast::FieldDefinition> &field_definitions,
                             uint32_t table_size) {
    ast::CreateStatement create_statement{ast::Table{table_name}, field_definitions};
    initializers::TableInitializer{create_statement, table_size}.initialize();
}

void testutils::cleanup_test_table(const std::string &table_name) {
    auto file_name = storage_utils::file_name_from_table_name(table_name);
    if (std::filesystem::exists(file_name)) {
        std::filesystem::remove(file_name);
    }
}
