#ifndef MATCHBOXDB_INSERT_ACTION_H
#define MATCHBOXDB_INSERT_ACTION_H

#include <cstdint>
#include <vector>
#include "../metadata.h"

namespace page_cache {
class PageCache;
}

namespace ast {
class InsertStatement;
}

namespace actions {
class InsertAction {
public:
    InsertAction(page_cache::PageCache &page_cache, const ast::InsertStatement &insert_statement);

    uint32_t save();
protected:
    page_cache::PageCache &_page_cache;
    const ast::InsertStatement &_insert_statement;

    [[nodiscard]] std::vector<metadata::DataType> build_attributes_for_tuple() const;
};
}

#endif //MATCHBOXDB_INSERT_ACTION_H
