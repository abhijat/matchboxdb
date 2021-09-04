#ifndef MATCHBOXDB_PAGE_SCANNER_H
#define MATCHBOXDB_PAGE_SCANNER_H


#include <istream>
#include "page_visitors.h"

namespace page_scan_utils {

class PageScanner {
public:
    PageScanner(std::istream &table_stream, page_visitors::PageVisitor &page_visitor);

    std::unordered_map<page::PageType, std::vector<page::PageId>> scan_pages();

protected:
    std::istream &_table_stream;
    page_visitors::PageVisitor &_page_visitor;
};

}

#endif //MATCHBOXDB_PAGE_SCANNER_H
