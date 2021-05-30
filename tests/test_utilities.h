#ifndef MATCHBOXDB_TEST_UTILITIES_H
#define MATCHBOXDB_TEST_UTILITIES_H


namespace testutils {

const std::string k_table_name{"employee"};
const std::string k_file_name{"employee.mbx"};
const uint32_t k_table_size_mb{1};

void create_test_table();

void cleanup_test_table();

}


#endif //MATCHBOXDB_TEST_UTILITIES_H
