#include "page_scanner.h"
#include "../streamutils.h"
#include "metadata_page.h"

page_scan_utils::PageScanner::PageScanner(std::istream &table_stream, page_visitors::PageVisitor &page_visitor)
    : _table_stream(table_stream), _page_visitor(page_visitor) {}

std::unordered_map<page::PageType, std::vector<page::PageId>> page_scan_utils::PageScanner::scan_pages() {
    std::unordered_map<page::PageType, std::vector<page::PageId>> page_directory{
        {page::PageType::Metadata, {}},
        {page::PageType::Data, {}},
        {page::PageType::RowMap, {}},
    };

    _table_stream.seekg(0, std::ios::beg);

    auto metadata_page = page::MetadataPage{stream_utils::read_page_from_stream(_table_stream)};
    for (auto i = 0; i < metadata_page.n_marked_pages(); ++i) {
        auto page_buffer = stream_utils::read_page_from_stream(_table_stream);
        _page_visitor.visit(page_buffer);

        auto page_type = page::page_type_from_buffer(page_buffer);
        auto page_id = page::page_id_from_buffer(page_buffer);
        page_directory[page_type].push_back(page_id);
    }

    return page_directory;
}
