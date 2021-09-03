#ifndef MATCHBOXDB_TEST_UTILITIES_H
#define MATCHBOXDB_TEST_UTILITIES_H


#include <gtest/gtest.h>
#include "../src/sql_parser/statement.h"

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


}


#endif //MATCHBOXDB_TEST_UTILITIES_H
