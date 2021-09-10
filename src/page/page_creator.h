#ifndef MATCHBOXDB_PAGE_CREATOR_H
#define MATCHBOXDB_PAGE_CREATOR_H


#include "metadata_page.h"

namespace page {

class PageCreator {
public:
    PageCreator(std::string table_name, MetadataPage *metadata_page);
    PageId create_page(page::PageType page_type);

protected:
    PageId create_data_page(std::fstream &f);

    PageId create_row_map_page(std::fstream &f);

protected:
    std::string _table_name;
    page::MetadataPage *_metadata_page;
};

}


#endif //MATCHBOXDB_PAGE_CREATOR_H
