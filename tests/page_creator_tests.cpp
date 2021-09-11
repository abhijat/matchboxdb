#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include "../src/storage/table_initializer.h"
#include "../src/page/page_creator.h"
#include "../src/sql_parser/field_definition.h"

class PageCreatorTests : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        ast::CreateStatement create_statement{ast::Table{"employee"}, {{"name", token::Kind::ColumnKindString}}};
        initializers::TableInitializer t{create_statement, 1};
        t.initialize();
    }

    static void TearDownTestSuite() {
        std::filesystem::remove("employee.mbx");
    }

};

TEST_F(PageCreatorTests, CreateDataPage) {
    std::ifstream i{"employee.mbx", std::ios::binary};
    page::MetadataPage metadata_page = stream_utils::read_nth_metadata_page(i);

    uint32_t original_data_page_count = metadata_page.n_data_pages();
    uint32_t original_marked_page_count = metadata_page.n_marked_pages();

    page::PageCreator page_creator{"employee", &metadata_page};
    auto page_id = page_creator.create_page(page::PageType::Data);

    ASSERT_EQ(page_id, 1);
    ASSERT_EQ(metadata_page.n_data_pages(), original_data_page_count + 1);
    ASSERT_EQ(metadata_page.n_marked_pages(), original_marked_page_count + 1);
}

TEST_F(PageCreatorTests, CreateMultiplePagesWithSeqPageId) {
    std::ifstream i{"employee.mbx", std::ios::binary};
    page::MetadataPage metadata_page = stream_utils::read_nth_metadata_page(i);

    page::PageCreator page_creator{"employee", &metadata_page};
    auto page_id = page_creator.create_page(page::PageType::Data);
    ASSERT_EQ(page_id, 1);

    page_id = page_creator.create_page(page::PageType::Data);
    ASSERT_EQ(page_id, 2);

    page_id = page_creator.create_page(page::PageType::Data);
    ASSERT_EQ(page_id, 3);

    page_id = page_creator.create_page(page::PageType::Data);
    ASSERT_EQ(page_id, 4);
}

TEST_F(PageCreatorTests, CreateRowMapPage) {
    std::ifstream i{"employee.mbx", std::ios::binary};
    page::MetadataPage metadata_page = stream_utils::read_nth_metadata_page(i);
    uint32_t original_row_map_page_count = metadata_page.n_rowmap_pages();
    uint32_t original_marked_page_count = metadata_page.n_marked_pages();

    page::PageCreator page_creator{"employee", &metadata_page};
    auto page_id = page_creator.create_page(page::PageType::RowMap);

    ASSERT_EQ(page_id, 1);
    ASSERT_EQ(metadata_page.n_rowmap_pages(), original_row_map_page_count + 1);
    ASSERT_EQ(metadata_page.n_marked_pages(), original_marked_page_count + 1);
}

TEST_F(PageCreatorTests, ThrowsWhenSpaceIsFull) {
    std::ifstream i{"employee.mbx", std::ios::binary};
    page::MetadataPage metadata_page = stream_utils::read_nth_metadata_page(i);
    auto unmarked_pages = metadata_page.n_unmarked_pages();

    page::PageCreator page_creator{"employee", &metadata_page};
    for (auto k = 0; k <= unmarked_pages; ++k) {
        if (!metadata_page.has_pages_available()) {
            ASSERT_THROW(page_creator.create_page(page::PageType::Data), std::out_of_range);
            break;
        } else {
            page_creator.create_page(page::PageType::Data);
        }
    }
}