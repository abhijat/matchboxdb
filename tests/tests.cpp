#include "gtest/gtest.h"
#include "../src/page_cache.h"

TEST(PageCacheTests, FileNameFromTableName) {
    std::string table_name{"foobar"};
    auto file_name = page_cache::file_name_from_table_name(table_name);
    ASSERT_EQ("foobar.mbx", file_name);
}

TEST(PageCacheTests, CacheKeyGen) {
    std::string table_name{"foobar"};
    uint32_t page_id{100};
    auto page_type = page::PageType::Metadata;
    auto key = page_cache::generate_cache_key(page_id, table_name, page_type);
    ASSERT_EQ("md::foobar::100", key);
}

