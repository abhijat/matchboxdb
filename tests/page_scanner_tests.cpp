#include <fstream>

#include <gtest/gtest.h>

#include "test_utilities.h"
#include "../src/page/page_scanner.h"
#include "../src/page/page_creator.h"

class ScannerTests : public testutils::TestsWithRealTable {
};

class CountingPageVisitor : public page_visitors::PageVisitor {
public:
    void visit(const stream_utils::ByteBuffer &buffer) override {
        pages_visited += 1;
        PageVisitor::visit(buffer);
    }

    void visit(const page::SlottedDataPage &data_page) override {
        data_pages += 1;
    }

    void visit(const page::MetadataPage &metadata_page) override {
        metadata_pages += 1;
    }

    void visit(const page::RowMappingPage &row_mapping_page) override {
        row_mapping_pages += 1;
    }

    int pages_visited{};
    int data_pages{};
    int metadata_pages{};
    int row_mapping_pages{};
};

TEST_F(ScannerTests, ScannerLooksAtAllMarkedPages) {

    {
        std::fstream fs{testutils::k_file_name, std::ios::binary | std::ios::in | std::ios::out};
        page::MetadataPage metadata_page{stream_utils::read_page_from_stream(fs)};

        page::PageCreator page_creator{testutils::k_table_name, &metadata_page};
        page_creator.create_page(page::PageType::Data);
        page_creator.create_page(page::PageType::Data);
        page_creator.create_page(page::PageType::Data);
        page_creator.create_page(page::PageType::Data);
        page_creator.create_page(page::PageType::RowMap);
        page_creator.create_page(page::PageType::RowMap);

        fs.seekp(0, std::ios::beg);
        stream_utils::write_page_to_stream(fs, metadata_page.buffer());
    }


    CountingPageVisitor counting_page_visitor{};

    std::ifstream ifs{testutils::k_file_name};
    page_scan_utils::PageScanner page_scanner{ifs, counting_page_visitor};

    page_scanner.scan_pages();

    ASSERT_EQ(counting_page_visitor.metadata_pages, 0);
    ASSERT_EQ(counting_page_visitor.row_mapping_pages, 2);
    ASSERT_EQ(counting_page_visitor.data_pages, 4);
    ASSERT_EQ(counting_page_visitor.pages_visited, 6);
}