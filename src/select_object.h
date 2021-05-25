#ifndef MATCHBOXDB_SELECT_OBJECT_H
#define MATCHBOXDB_SELECT_OBJECT_H

#include "page_cache.h"

namespace actions {
class SelectObject {
public:
    SelectObject(page_cache::PageCache &page_cache, const std::string &table_name, uint32_t row_id);

    tuple::Tuple get();

protected:
    page_cache::PageCache &_page_cache;
    std::string _table_name;
    uint32_t _row_id;

};
}

#endif //MATCHBOXDB_SELECT_OBJECT_H
