#include <gtest/gtest.h>

#include "../test_utilities.h"
#include "../../src/page/page_cache.h"
#include "../../src/sql_parser/parser.h"
#include "../../src/sql_parser/create_statement.h"
#include "../../src/storage/table_initializer.h"

class DescribeStatementTestSuite : public testutils::TestsWithRealTable {
};

TEST_F(DescribeStatementTestSuite, DescribeSingleTable) {
    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    auto result = std::get<std::string>(testutils::execute(executor, "DESCRIBE TABLE employee"));

    std::istringstream i{result};
    std::vector<std::string> tokens{};

    std::copy(std::istream_iterator<std::string>{i}, std::istream_iterator<std::string>{},
              std::back_inserter(tokens));

    std::vector<std::string> expected{"employee", "(", "name", "String", "age", "UnsignedInt", ")"};
    ASSERT_EQ(tokens, expected);
}

TEST_F(DescribeStatementTestSuite, MultipleTables) {

    page_cache::PageCache cache{5, std::vector<std::string>{testutils::k_table_name}};
    command_executor::CommandExecutor executor{cache};

    testutils::create_test_table("pets",
                                 {{"name",         token::Kind::ColumnKindString},
                                  {"is_dangerous", token::Kind::ColumnKindBoolean}}, testutils::k_table_size_mb);
    cache.add_new_table("pets");

    auto result = std::get<std::string>(testutils::execute(executor, "DESCRIBE TABLE"));

    std::istringstream i{result};
    std::vector<std::string> tokens{};

    std::copy_if(std::istream_iterator<std::string>{i}, std::istream_iterator<std::string>{},
                 std::back_inserter(tokens), [](const auto &token) {
            return token != "(" && token != ")";
        });

    std::vector<std::string> expected{"employee", "name", "String", "age", "UnsignedInt", "pets", "name", "String",
                                      "is_dangerous", "Boolean"};

    ASSERT_EQ(expected, tokens);

    testutils::cleanup_test_table("pets");
}
