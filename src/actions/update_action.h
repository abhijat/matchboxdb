#ifndef MATCHBOXDB_UPDATE_ACTION_H
#define MATCHBOXDB_UPDATE_ACTION_H

#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include "../tuple.h"
#include "../page/slotted_data_page.h"

namespace page_cache {
class PageCache;
}

namespace ast {
class UpdateStatement;

class Expression;
}

namespace metadata {
class Metadata;
}

namespace actions {
class UpdateAction {
public:
    UpdateAction(page_cache::PageCache &page_cache, const ast::UpdateStatement &update_statement);

    uint32_t update();

protected:
    [[nodiscard]] std::unordered_map<std::string, metadata::DataType>
    generate_updates(const tuple::Tuple &tuple, const metadata::Metadata &metadata) const;

    [[nodiscard]] tuple::Tuple updated_tuple(const tuple::Tuple &tuple, const metadata::Metadata &metadata) const;

    void store_updated_tuple(const std::string &table_name, const metadata::Metadata &metadata,
                             page::SlottedDataPage *data_page, const tuple::Tuple &tuple) const;

protected:
    page_cache::PageCache &_page_cache;
    const ast::UpdateStatement &_update_statement;
    ast::Expression *_where;


};
}

#endif //MATCHBOXDB_UPDATE_ACTION_H
