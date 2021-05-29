#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "../src/table_initializer.h"
#include "../src/streamutils.h"
#include "../src/metadata_page.h"
#include "../src/slotted_data_page.h"
#include "../src/row_mapping_page.h"

class TableInitializationTests : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        path_to_file = "foobar.mbx";
        file_size = 32;
        metadata::Metadata md{{"name",                 "age"},
                              {metadata::Kind::String, metadata::Kind::UnsignedInt}};
        initializers::TableInitializer{"foobar", path_to_file, file_size, md}.initialize();
    }

    static void TearDownTestSuite() {
        if (std::filesystem::exists(path_to_file)) {
            std::filesystem::remove(path_to_file);
        }
    }

    static std::string path_to_file;
    static uint32_t file_size;
};

std::string TableInitializationTests::path_to_file{};
uint32_t TableInitializationTests::file_size{};


TEST_F(TableInitializationTests, FileIsCreated) {
    ASSERT_TRUE(std::filesystem::exists(path_to_file));
    ASSERT_EQ(std::filesystem::file_size(path_to_file), file_size * 1024 * 1024);
}

TEST_F(TableInitializationTests, PageCalculations) {
    std::ifstream i{path_to_file, std::ios::binary};
    auto metadata_page = stream_utils::read_nth_metadata_page(i);
    ASSERT_EQ(metadata_page.page_type(), page::PageType::Metadata);
    ASSERT_EQ(metadata_page.n_data_pages(), 2048);
    ASSERT_EQ(metadata_page.n_rowmap_pages(), 128);
}

TEST_F(TableInitializationTests, ReadRandomDataPages) {
    std::ifstream i{path_to_file, std::ios::binary};
    auto data_page = stream_utils::read_nth_data_page(i, 100);
    ASSERT_EQ(data_page.page_type(), page::PageType::Data);
}

TEST_F(TableInitializationTests, ReadRandomRowmapPages) {
    std::ifstream i{path_to_file, std::ios::binary};
    auto row_map_page = stream_utils::read_nth_row_mapping_page(i, 100);
    ASSERT_EQ(row_map_page.page_type(), page::PageType::RowMap);
}
