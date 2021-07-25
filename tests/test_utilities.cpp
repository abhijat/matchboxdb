#include <filesystem>
#include "test_utilities.h"
#include "../src/metadata.h"
#include "../src/table_initializer.h"

void testutils::create_test_table() {
    metadata::Metadata md{{"name",                 "age"},
                          {metadata::Kind::String, metadata::Kind::UnsignedInt}};
    initializers::TableInitializer{k_table_name, k_file_name, k_table_size_mb, md}.initialize();
}

void testutils::cleanup_test_table() {
    if (std::filesystem::exists(k_file_name)) {
        std::filesystem::remove(k_file_name);
    }
}

void testutils::TestsWithRealTable::SetUp() {
    create_test_table();
}

void testutils::TestsWithRealTable::TearDown() {
    cleanup_test_table();
}
