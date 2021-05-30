#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <fstream>

#include "test_utilities.h"
#include "../src/page_cache.h"

class TestPageCache : public page_cache::PageCache {
public:
    TestPageCache(uint32_t max_size, const std::vector<std::string> &tables) : PageCache(max_size, tables) {}

    uint32_t dirty_page_size(const std::string &table_name) const {
        return _dirty_pages.at(table_name).size();
    }

    std::string last_evicted{};

protected:
    std::string evict() override {
        last_evicted = page_cache::PageCache::evict();
        return last_evicted;
    }

};

class PageCacheTests : public ::testing::Test {
protected:
    void SetUp() override {
        testutils::create_test_table();
    }

    void TearDown() override {
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

TEST_F(PageCacheTests, WriteDirtyPagesToDisk) {
    TestPageCache pc{5, {testutils::k_table_name}};
    const auto&[r, d] = pc.get_pages_for_data_size(testutils::k_table_name, 32);

    ASSERT_EQ(pc.dirty_page_size(testutils::k_table_name), 3);

    pc.write_dirty_pages(testutils::k_table_name);

    ASSERT_EQ(pc.dirty_page_size(testutils::k_table_name), 0);

    std::ifstream f{testutils::k_file_name, std::ios::binary};
    auto buffer = stream_utils::read_page_from_stream(f, d->page_id());
    ASSERT_EQ(page::page_type_from_buffer(buffer), page::PageType::Data);

    buffer = stream_utils::read_page_from_stream(f, r->page_id());
    ASSERT_EQ(page::page_type_from_buffer(buffer), page::PageType::RowMap);
}

TEST_F(PageCacheTests, WriteDirtyPagesUpdatesTableMetadata) {
    page_cache::PageCache pc{5, {testutils::k_table_name}};
    auto metadata_page = pc.metadata_page_for_table(testutils::k_table_name);
    const auto&[r, d] = pc.get_pages_for_data_size(testutils::k_table_name, 32);

    ASSERT_EQ(metadata_page->n_data_pages(), 1);
    ASSERT_EQ(metadata_page->n_rowmap_pages(), 1);
    {
        std::ifstream f{testutils::k_file_name, std::ios::binary};
        auto metadata_on_disk = page::MetadataPage{stream_utils::read_page_from_stream(f)};
        ASSERT_EQ(metadata_on_disk.n_data_pages(), 0);
        ASSERT_EQ(metadata_on_disk.n_rowmap_pages(), 0);
        ASSERT_EQ(metadata_on_disk.n_marked_pages(), 0);
    }

    pc.write_dirty_pages(testutils::k_table_name);
    {
        std::ifstream f{testutils::k_file_name, std::ios::binary};
        auto metadata_on_disk = page::MetadataPage{stream_utils::read_page_from_stream(f)};
        ASSERT_EQ(metadata_on_disk.n_data_pages(), 1);
        ASSERT_EQ(metadata_on_disk.n_rowmap_pages(), 1);
        ASSERT_EQ(metadata_on_disk.n_marked_pages(), 2);
    }
}

TEST_F(PageCacheTests, CacheEviction) {
    TestPageCache pc{2, {testutils::k_table_name}};
    ASSERT_TRUE(pc.last_evicted.empty());

    // this should trigger metadata page eviction:
    const auto&[r, d] = pc.get_pages_for_data_size(testutils::k_table_name, 32);
    ASSERT_EQ(pc.last_evicted, page_cache::generate_cache_key(0, testutils::k_table_name, page::PageType::Metadata));

    // this should trigger data page eviction:
    pc.metadata_page_for_table(testutils::k_table_name);
    ASSERT_EQ(pc.last_evicted, page_cache::generate_cache_key(1, testutils::k_table_name, page::PageType::Data));
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
