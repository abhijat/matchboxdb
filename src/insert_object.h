#ifndef MATCHBOXDB_INSERT_OBJECT_H
#define MATCHBOXDB_INSERT_OBJECT_H

#include <string>
#include "tuple.h"

namespace page_cache {
class PageCache;
}

namespace actions {

class InsertObject {
public:
    InsertObject(page_cache::PageCache &page_cache, std::string table_name, tuple::Tuple tuple);

    uint32_t save();

protected:
    page_cache::PageCache &_page_cache;
    std::string _table_name;
    tuple::Tuple _tuple;
};

}

#endif //MATCHBOXDB_INSERT_OBJECT_H
