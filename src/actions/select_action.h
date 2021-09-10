#ifndef MATCHBOXDB_SELECT_ACTION_H
#define MATCHBOXDB_SELECT_ACTION_H

#include <string>
#include <vector>

#include "../metadata.h"
#include "../tuple.h"

namespace page_cache {
class PageCache;
}

namespace tuple {
class Tuple;
}

namespace ast {
class SelectStatement;

class Expression;
}

namespace actions {

class SelectAction {
public:
    SelectAction(page_cache::PageCache &page_cache, const ast::SelectStatement &select_statement);

    std::vector<tuple::TupleView> list();

protected:
    page_cache::PageCache &_page_cache;
    ast::Expression *_where;
    const ast::SelectStatement &_select_statement;
    std::vector<std::string> _columns{};
};

}

#endif //MATCHBOXDB_SELECT_ACTION_H
