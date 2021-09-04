#include <gtest/gtest.h>

#include "test_utilities.h"
#include "../src/command_executor.h"
#include "../src/page_cache.h"
#include "../src/sql_parser/parser.h"


class InsertStatementTestSuite : public testutils::TestsWithRealTable {
};

TEST_F(InsertStatementTestSuite, SimpleInsertTest) {
    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    auto statement = parser::Parser{lexer::Lexer{R"(INSERT INTO employee VALUES ("cujo", 7);)"}}.parse();

    auto row_id = executor.execute(*statement);

    ASSERT_EQ(std::get<uint32_t>(row_id), 1);

    statement = parser::Parser{lexer::Lexer{R"(SELECT name, age FROM employee)"}}.parse();

    auto result = executor.execute(*statement);

    auto tuples = std::get<std::vector<tuple::Tuple>>(result);
    ASSERT_EQ(tuples.size(), 1);

    auto attrs = tuples[0].attributes();
    ASSERT_EQ(std::get<std::string>(attrs[0]), "cujo");
    ASSERT_EQ(std::get<uint32_t>(attrs[1]), 7);
}
