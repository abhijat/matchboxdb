#include <gtest/gtest.h>

#include "test_utilities.h"
#include "../src/page/page_cache.h"
#include "../src/sql_parser/parser.h"


class SelectStatementTestSuite : public testutils::TestsWithRealTable {
};

TEST_F(SelectStatementTestSuite, SimpleSelect) {
    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    testutils::execute(executor, R"(INSERT INTO employee VALUES ("cujo", 7);)");

    auto result = testutils::execute(executor, R"(SELECT name, age FROM employee)");
    auto tuples = std::get<std::vector<tuple::TupleView>>(result);
    ASSERT_EQ(tuples.size(), 1);

    testutils::assert_value<std::string>(tuples[0]["name"], "cujo");
    testutils::assert_value<uint32_t>(tuples[0]["age"], 7);
}

TEST_F(SelectStatementTestSuite, SelectWithCriteria) {
    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    testutils::execute(executor, R"(INSERT INTO employee VALUES ("cujo", 7);)");
    testutils::execute(executor, R"(INSERT INTO employee VALUES ("alucard", 1000);)");
    testutils::execute(executor, R"(INSERT INTO employee VALUES ("ivan karamazov", 24);)");

    auto result = testutils::execute(executor, R"(SELECT name, age FROM employee WHERE age > 20)");

    auto tuples = std::get<std::vector<tuple::TupleView>>(result);
    ASSERT_EQ(tuples.size(), 2);

    testutils::assert_value<std::string>(tuples[0]["name"], "alucard");
    testutils::assert_value<uint32_t>(tuples[0]["age"], 1000);

    testutils::assert_value<std::string>(tuples[1]["name"], "ivan karamazov");
    testutils::assert_value<uint32_t>(tuples[1]["age"], 24);
}

TEST_F(SelectStatementTestSuite, SelectSpecificColumns) {
    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    testutils::execute(executor, R"(INSERT INTO employee VALUES ("cujo", 7);)");

    auto result = testutils::execute(executor, R"(SELECT age, name FROM employee)");
    auto tuples = std::get<std::vector<tuple::TupleView>>(result);
    ASSERT_EQ(tuples.size(), 1);

    testutils::assert_value<std::string>(tuples[0]["name"], "cujo");
    testutils::assert_value<uint32_t>(tuples[0]["age"], 7);

    result = testutils::execute(executor, R"(SELECT age FROM employee)");
    tuples = std::get<std::vector<tuple::TupleView>>(result);
    ASSERT_EQ(tuples.size(), 1);

    testutils::assert_value<uint32_t>(tuples[0]["age"], 7);
}
