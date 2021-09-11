#include <gtest/gtest.h>
#include <filesystem>

#include "../test_utilities.h"
#include "../../src/page/page_cache.h"
#include "../../src/storage/table_initializer.h"

TEST(CreateTableTests, CreateTable) {
    page_cache::PageCache pc{initializers::k_table_size_in_mb, {}};
    command_executor::CommandExecutor executor{pc};
    testutils::execute(
        executor,
        R"(CREATE TABLE automobiles (brand STRING, fuel_capacity UNSIGNED_INT, is_manual BOOLEAN, n_wheels UNSIGNED_INT);)");

    testutils::execute(executor, R"(INSERT INTO automobiles VALUES ("horse buggy", 10, true, 6);)");
    auto result = testutils::execute(executor, R"(SELECT fuel_capacity FROM automobiles)");

    auto tuple = std::get<std::vector<tuple::TupleView>>(result)[0];

    ASSERT_TRUE(std::get<bool>(tuple["is_manual"]));

    ASSERT_TRUE(std::filesystem::exists("automobiles.mbx"));
    std::filesystem::remove("automobiles.mbx");
}