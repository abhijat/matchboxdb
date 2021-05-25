#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

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

TEST(MetadataPageTests, PageToBufferToPage) {
    std::vector<std::string> columns{"name", "age"};
    std::vector<metadata::Kind> kinds{metadata::Kind::String, metadata::Kind::UnsignedInt};
    page::MetadataPage m{"foobar", columns, kinds, 100, 10, 22};

    auto buffer = m.buffer();
    ASSERT_EQ(buffer.size(), page::k_page_size);

    page::MetadataPage n{buffer};

    ASSERT_EQ(n.n_rowmap_pages(), 10);
    ASSERT_EQ(n.n_data_pages(), 100);
    ASSERT_EQ(n.max_row_id(), 22);
    ASSERT_EQ(n.table_name(), "foobar");
    ASSERT_THAT(n.column_names(), ::testing::ContainerEq(columns));
    ASSERT_THAT(n.column_kinds(), ::testing::ContainerEq(kinds));
}

TEST(SlottedDataPageTests, EmptyPage) {
    page::SlottedDataPage slotted_data_page{page::k_header_size, 0, page::k_page_size};
    auto buffer = slotted_data_page.empty_page();

    page::SlottedDataPage new_page{buffer};
    ASSERT_EQ(new_page.page_id(), slotted_data_page.page_id());
    ASSERT_EQ(new_page.header_size(), slotted_data_page.header_size());
    ASSERT_EQ(new_page.slot_end_marker(), new_page.header_size() + sizeof(uint32_t) + sizeof(uint32_t));
    ASSERT_EQ(new_page.tuple_begin_marker(), page::k_page_size);
}