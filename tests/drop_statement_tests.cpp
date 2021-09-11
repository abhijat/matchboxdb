#include <gtest/gtest.h>
#include <filesystem>

#include "test_utilities.h"
#include "../src/page/page_cache.h"
#include "../src/storage/table_initializer.h"
#include "../src/storage/utils.h"

TEST(DropTableTests, DropTable) {
    page_cache::PageCache pc{initializers::k_table_size_in_mb, {}};
    command_executor::CommandExecutor executor{pc};

    testutils::execute(executor, R"(CREATE TABLE automobiles (brand STRING);)");
    ASSERT_TRUE(std::filesystem::exists(storage_utils::file_name_from_table_name("automobiles")));

    testutils::execute(executor, R"(DROP TABLE automobiles;)");
    ASSERT_FALSE(std::filesystem::exists(storage_utils::file_name_from_table_name("automobiles")));
}
