#include <gtest/gtest.h>

#include "test_utilities.h"
#include "../src/page/page_cache.h"
#include "../src/command_executor.h"
#include "../src/sql_parser/parser.h"

class DeleteStatementTestSuite : public testutils::TestsWithRealTable {
};

command_executor::CommandExecutionResult execute(command_executor::CommandExecutor &executor,
                                                 const std::string &statement) {
    auto parsed = parser::Parser{lexer::Lexer{statement}}.parse();
    return executor.execute(*parsed);
}

TEST_F(DeleteStatementTestSuite, SimpleDeleteTest) {
    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    execute(executor, R"(INSERT INTO employee VALUES ("cujo", 7);)");
    execute(executor, R"(INSERT INTO employee VALUES ("alucard", 1000);)");
    auto result = execute(executor, R"(DELETE FROM employee)");
    ASSERT_EQ(std::get<uint32_t>(result), 2);

    result = execute(executor, R"(SELECT name, age FROM employee)");
    ASSERT_EQ(std::get<std::vector<tuple::Tuple>>(result).size(), 0);
}

TEST_F(DeleteStatementTestSuite, DeleteWithWhereClause) {
    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    execute(executor, R"(INSERT INTO employee VALUES ("cujo", 7);)");
    execute(executor, R"(INSERT INTO employee VALUES ("alucard", 1000);)");
    auto result = execute(executor, R"(DELETE FROM employee WHERE age > 500)");
    ASSERT_EQ(std::get<uint32_t>(result), 1);

    result = execute(executor, R"(SELECT name, age FROM employee)");
    auto tuples = std::get<std::vector<tuple::Tuple>>(result);
    ASSERT_EQ(tuples.size(), 1);
    auto attrs = tuples[0].attributes();
    ASSERT_EQ(std::get<std::string>(attrs[0]), "cujo");
    ASSERT_EQ(std::get<uint32_t>(attrs[1]), 7);

    result = execute(executor, R"(DELETE FROM employee WHERE age > 500)");
    ASSERT_EQ(std::get<uint32_t>(result), 0);
}
