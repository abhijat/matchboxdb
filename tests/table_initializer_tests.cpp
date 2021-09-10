#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "../src/table_initializer.h"
#include "../src/streamutils.h"
#include "../src/page/metadata_page.h"
#include "../src/sql_parser/token.h"
#include "../src/sql_parser/field_definition.h"

class TableInitializationTests : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        path_to_file = "foobar.mbx";
        file_size = 32;
        ast::CreateStatement create_statement{ast::Table{"foobar"}, {
            {"name", token::Kind::ColumnKindString},
            {"age", token::Kind::ColumnKindUnsignedInteger},
        }};
        initializers::TableInitializer{create_statement, file_size}.initialize();
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
    ASSERT_EQ(metadata_page.n_data_pages(), 0);
    ASSERT_EQ(metadata_page.n_rowmap_pages(), 0);
}
