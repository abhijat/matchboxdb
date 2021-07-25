#include "page_visitors.h"
#include "page.h"

#include "slotted_data_page.h"
#include "row_mapping_page.h"
#include "metadata_page.h"

void page_visitors::PageVisitor::visit(const stream_utils::ByteBuffer &buffer) {
    auto page_type = page::page_type_from_buffer(buffer);
    switch (page_type) {
        case page::PageType::Data:
            visit(page::SlottedDataPage{buffer});
            break;
        case page::PageType::RowMap:
            visit(page::RowMappingPage{buffer});
            break;
        case page::PageType::Metadata:
            visit(page::MetadataPage{buffer});
            break;
    }
}

void page_visitors::FreePageCollector::visit(const page::SlottedDataPage &data_page) {
    _free_data_pages.emplace_back(data_page.page_id(), data_page.free_space());
}

void page_visitors::FreePageCollector::visit(const page::MetadataPage &metadata_page) {

}

void page_visitors::FreePageCollector::visit(const page::RowMappingPage &row_mapping_page) {
    _free_row_map_pages.emplace_back(row_mapping_page.page_id(), row_mapping_page.free_space());
}

void page_visitors::FreePageCollector::visit(const stream_utils::ByteBuffer &buffer) {
    if (page::free_space_from_buffer(buffer) > 0) {
        PageVisitor::visit(buffer);
    }
}

std::vector<page::FreePageInfo> page_visitors::FreePageCollector::free_data_pages() const {
    return _free_data_pages;
}

std::vector<page::FreePageInfo> page_visitors::FreePageCollector::free_row_map_pages() const {
    return _free_row_map_pages;
}
