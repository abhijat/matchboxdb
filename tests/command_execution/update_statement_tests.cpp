#include <gtest/gtest.h>

#include "../test_utilities.h"
#include "../../src/page/page_cache.h"
#include "../../src/sql_parser/parser.h"

class UpdateStatementTestSuite : public testutils::TestsWithRealTable {
};

TEST_F(UpdateStatementTestSuite, SimpleUpdateTest) {
    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    testutils::execute(executor, R"(INSERT INTO employee VALUES ("cujo", 7);)");
    testutils::execute(executor, R"(INSERT INTO employee VALUES ("alucard", 1000);)");
    auto result = testutils::execute(executor, R"(UPDATE employee SET name = "linoleum floors", age = 0;)");
    ASSERT_EQ(std::get<uint32_t>(result), 2);

    result = testutils::execute(executor, R"(SELECT name, age FROM employee)");
    const auto &tuples = std::get<std::vector<tuple::TupleView>>(result);
    for (const auto &tuple: tuples) {
        auto name = tuple["name"];
        auto age = tuple["age"];
        ASSERT_EQ(std::get<std::string>(name), "linoleum floors");
        ASSERT_EQ(std::get<uint32_t>(age), 0);
    }

    ASSERT_EQ(tuples.size(), 2);
}

TEST_F(UpdateStatementTestSuite, UpdateWithFilter) {
    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    testutils::execute(executor, R"(INSERT INTO employee VALUES ("cujo", 7);)");
    testutils::execute(executor, R"(INSERT INTO employee VALUES ("alucard", 1000);)");
    auto result = testutils::execute(executor,
                                     R"(UPDATE employee SET name = "linoleum floors", age = 0 WHERE age > 500;)");
    ASSERT_EQ(std::get<uint32_t>(result), 1);

    {
        result = testutils::execute(executor, R"(SELECT name, age FROM employee WHERE age = 0)");
        const auto &tuples = std::get<std::vector<tuple::TupleView>>(result);
        for (const auto &tuple: tuples) {
            ASSERT_EQ(std::get<std::string>(tuple["name"]), "linoleum floors");
            ASSERT_EQ(std::get<uint32_t>(tuple["age"]), 0);
        }

        ASSERT_EQ(tuples.size(), 1);
    }

    {
        result = testutils::execute(executor, R"(SELECT name, age FROM employee WHERE age != 0)");
        const auto &tuples = std::get<std::vector<tuple::TupleView>>(result);
        for (const auto &tuple: tuples) {
            ASSERT_EQ(std::get<std::string>(tuple["name"]), "cujo");
            ASSERT_EQ(std::get<uint32_t>(tuple["age"]), 7);
        }

        ASSERT_EQ(tuples.size(), 1);
    }
}
