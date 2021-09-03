#include <filesystem>
#include "test_utilities.h"
#include "../src/metadata.h"
#include "../src/table_initializer.h"
#include "../src/sql_parser/token.h"
#include "../src/sql_parser/field_definition.h"
#include "../src/sql_parser/parser.h"

void testutils::create_test_table() {
    ast::CreateStatement create_statement{ast::Table{k_table_name}, {
        {"name", token::Kind::ColumnKindString},
        {"age", token::Kind::ColumnKindUnsignedInteger},
    }};
    initializers::TableInitializer{create_statement, k_table_size_mb}.initialize();
}

void testutils::cleanup_test_table() {
    if (std::filesystem::exists(k_file_name)) {
        std::filesystem::remove(k_file_name);
    }
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
