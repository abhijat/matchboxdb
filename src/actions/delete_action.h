#ifndef MATCHBOXDB_DELETE_ACTION_H
#define MATCHBOXDB_DELETE_ACTION_H

#include <cstdint>

namespace page_cache {
class PageCache;
}

namespace ast {
class DeleteStatement;

class Expression;
}

namespace actions {

class DeleteAction {
public:
    DeleteAction(page_cache::PageCache &page_cache, const ast::DeleteStatement &delete_statement);

    uint32_t apply_delete();

protected:
    page_cache::PageCache &_page_cache;
    ast::Expression *_where;
    const ast::DeleteStatement &_delete_statement;
};

}

#endif //MATCHBOXDB_DELETE_ACTION_H
