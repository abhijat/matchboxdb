#include <gtest/gtest.h>

#include "test_utilities.h"
#include "../src/command_executor.h"
#include "../src/page_cache.h"
#include "../src/sql_parser/parser.h"
#include "../src/sql_parser/statement.h"


class InsertStatementTestSuite : public testutils::TestsWithRealTable {
};

TEST_F(InsertStatementTestSuite, SimpleInsertTest) {
    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    auto statement = parser::Parser{lexer::Lexer{R"(INSERT INTO employee VALUES ("cujo", 7);)"}}.parse();

    executor.execute(*statement);

    statement = parser::Parser{lexer::Lexer{R"(SELECT name, age FROM employee)"}}.parse();

    executor.execute(*statement);
}