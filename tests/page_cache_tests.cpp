#include <gtest/gtest.h>
#include <fstream>
#include "test_utilities.h"
#include "../src/page_cache.h"

class PageCacheTests : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        testutils::create_test_table();
    }

    static void TearDownTestSuite() {
        testutils::cleanup_test_table();
    }
};


TEST_F(PageCacheTests, MetadataPageForTable) {
    page_cache::PageCache pc{5, {testutils::k_table_name}};
    auto metdata_page = pc.metadata_page_for_table(testutils::k_table_name);
    ASSERT_EQ(metdata_page->table_name(), testutils::k_table_name);
}

TEST_F(PageCacheTests, FirstCallCreatesPages) {
    page_cache::PageCache pc{5, {testutils::k_table_name}};
    auto metdata_page = pc.metadata_page_for_table(testutils::k_table_name);

    ASSERT_EQ(metdata_page->n_marked_pages(), 0);
    const auto&[r, d] = pc.get_pages_for_data_size(testutils::k_table_name, 32);
    ASSERT_EQ(metdata_page->n_marked_pages(), 2);
    ASSERT_EQ(d->page_id(), 1);
    ASSERT_EQ(d->page_type(), page::PageType::Data);

    ASSERT_EQ(r->page_id(), 2);
    ASSERT_EQ(r->page_type(), page::PageType::RowMap);
}

TEST_F(PageCacheTests, WriteDirtyPages) {
    page_cache::PageCache pc{5, {testutils::k_table_name}};
    const auto&[r, d] = pc.get_pages_for_data_size(testutils::k_table_name, 32);

    pc.write_dirty_pages(testutils::k_table_name);

    std::ifstream f{testutils::k_file_name, std::ios::binary};

    auto buffer = stream_utils::read_page_from_stream(f, d->page_id());
    ASSERT_EQ(page::page_type_from_buffer(buffer), page::PageType::Data);

    buffer = stream_utils::read_page_from_stream(f, r->page_id());
    ASSERT_EQ(page::page_type_from_buffer(buffer), page::PageType::RowMap);
}

TEST(PageCacheUtilityTests, FileNameFromTableName) {
    std::string table_name{"foobar"};
    auto file_name = page_cache::file_name_from_table_name(table_name);
    ASSERT_EQ("foobar.mbx", file_name);
}

TEST(PageCacheUtilityTests, CacheKeyGen) {
    std::string table_name{"foobar"};
    uint32_t page_id{100};
    auto page_type = page::PageType::Metadata;
    auto key = page_cache::generate_cache_key(page_id, table_name, page_type);
    ASSERT_EQ("md::foobar::100", key);
}
