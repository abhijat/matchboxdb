#ifndef MATCHBOXDB_DROP_STATEMENT_H
#define MATCHBOXDB_DROP_STATEMENT_H

#include "statement.h"
#include "table.h"

namespace ast {

class DropStatement : public Statement {
public:
    explicit DropStatement(Table table);

    [[nodiscard]] std::string table_name() const;

    void repr(std::ostream &os) const override;

    void accept(StatementVisitor &visitor) const override;

protected:
    Table _table;
};

}

#endif //MATCHBOXDB_DROP_STATEMENT_H
