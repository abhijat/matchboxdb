#include "page_scanner.h"
#include "streamutils.h"
#include "metadata_page.h"

page_scan_utils::PageScanner::PageScanner(std::istream &table_stream, page_visitors::PageVisitor &page_visitor)
    : _table_stream(table_stream), _page_visitor(page_visitor) {}

void page_scan_utils::PageScanner::scan_pages() {
    _table_stream.seekg(0, std::ios::beg);

    auto metadata_page = page::MetadataPage{stream_utils::read_page_from_stream(_table_stream)};
    for (auto i = 0; i < metadata_page.n_marked_pages(); ++i) {
        _page_visitor.visit(stream_utils::read_page_from_stream(_table_stream));
    }
}
