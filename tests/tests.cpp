#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "../src/page/page_cache.h"

TEST(MetadataPageTests, PageToBufferToPage) {
    std::vector<std::string> columns{"name", "age"};
    std::vector<metadata::Kind> kinds{metadata::Kind::String, metadata::Kind::UnsignedInt};
    page::MetadataPage m{"foobar", columns, kinds, 100, 22, 0};

    auto buffer = m.buffer();
    ASSERT_EQ(buffer.size(), page::k_page_size);

    page::MetadataPage n{buffer};

    ASSERT_EQ(n.n_data_pages(), 100);
    ASSERT_EQ(n.max_row_id(), 22);
    ASSERT_EQ(n.table_name(), "foobar");
    ASSERT_THAT(n.column_names(), ::testing::ContainerEq(columns));
    ASSERT_THAT(n.column_kinds(), ::testing::ContainerEq(kinds));
}

TEST(TupleTests, Serialization) {
    std::vector<std::string> names{"name", "age", "is_employed"};
    std::vector<metadata::Kind> types{metadata::Kind::String, metadata::Kind::UnsignedInt, metadata::Kind::Boolean};
    metadata::Metadata m{names, types};

    std::vector<metadata::DataType> attributes{std::string{"abhijat"}, uint32_t{38}, true};
    tuple::Tuple t{attributes};

    auto buffer = t.data();

    tuple::Tuple a{buffer, m};
    const auto &attrs = a.attributes();
    ASSERT_THAT(attrs[0], ::testing::VariantWith<std::string>("abhijat"));
    ASSERT_THAT(attrs[1], ::testing::VariantWith<uint32_t>(38));
    ASSERT_THAT(attrs[2], ::testing::VariantWith<bool>(true));
}

TEST(PageTests, PageTypeFromBuffer) {
    page::SlottedDataPage slotted_data_page{0, page::k_page_size};
    ASSERT_EQ(page::PageType::Data, page::page_type_from_buffer(slotted_data_page.empty_page()));

    page::MetadataPage metadata_page{"", {}, {}, 0, 0, 0};
    ASSERT_EQ(page::PageType::Metadata, page::page_type_from_buffer(metadata_page.buffer()));
}
