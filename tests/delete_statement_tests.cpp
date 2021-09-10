#include <gtest/gtest.h>

#include "test_utilities.h"
#include "../src/page/page_cache.h"
#include "../src/sql_parser/parser.h"

class DeleteStatementTestSuite : public testutils::TestsWithRealTable {
};

TEST_F(DeleteStatementTestSuite, SimpleDeleteTest) {
    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    testutils::execute(executor, R"(INSERT INTO employee VALUES ("cujo", 7);)");
    testutils::execute(executor, R"(INSERT INTO employee VALUES ("alucard", 1000);)");
    auto result = testutils::execute(executor, R"(DELETE FROM employee)");
    ASSERT_EQ(std::get<uint32_t>(result), 2);

    result = testutils::execute(executor, R"(SELECT name, age FROM employee)");
    ASSERT_EQ(std::get<std::vector<tuple::TupleView>>(result).size(), 0);
}

TEST_F(DeleteStatementTestSuite, DeleteWithWhereClause) {
    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    testutils::execute(executor, R"(INSERT INTO employee VALUES ("cujo", 7);)");
    testutils::execute(executor, R"(INSERT INTO employee VALUES ("alucard", 1000);)");
    auto result = testutils::execute(executor, R"(DELETE FROM employee WHERE age > 500)");
    ASSERT_EQ(std::get<uint32_t>(result), 1);

    result = testutils::execute(executor, R"(SELECT name, age FROM employee)");
    auto tuples = std::get<std::vector<tuple::TupleView>>(result);
    ASSERT_EQ(tuples.size(), 1);
    auto tuple = tuples[0];
    ASSERT_EQ(std::get<std::string>(tuple["name"]), "cujo");
    ASSERT_EQ(std::get<uint32_t>(tuple["age"]), 7);

    result = testutils::execute(executor, R"(DELETE FROM employee WHERE age > 500)");
    ASSERT_EQ(std::get<uint32_t>(result), 0);
}
