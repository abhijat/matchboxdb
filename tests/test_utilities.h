#ifndef MATCHBOXDB_TEST_UTILITIES_H
#define MATCHBOXDB_TEST_UTILITIES_H


#include <gtest/gtest.h>
#include "../src/sql_parser/statement.h"
#include "../src/command_executor.h"

namespace testutils {

const std::string k_table_name{"employee"};
const std::string k_file_name{"employee.mbx"};
const uint32_t k_table_size_mb{1};

void create_test_table();

void cleanup_test_table();

class TestsWithRealTable : public ::testing::Test {
protected:
    void SetUp() override;

    void TearDown() override;
};

std::unique_ptr<ast::Statement> parse(const std::string &s);

command_executor::CommandExecutionResult execute(command_executor::CommandExecutor &executor,
                                                 const std::string &statement);

template<typename T>
void assert_value(const metadata::DataType &data_type, const T &t) {
    ASSERT_EQ(std::get<T>(data_type), t) << "value did not match: " << t;
}

}


#endif //MATCHBOXDB_TEST_UTILITIES_H
