#ifndef MATCHBOXDB_STATEMENT_H
#define MATCHBOXDB_STATEMENT_H

#include "node.h"

namespace ast {

class StatementVisitor;

class Statement : public Node {
public:
    virtual void accept(StatementVisitor &visitor) const = 0;
};
}

#endif //MATCHBOXDB_STATEMENT_H
