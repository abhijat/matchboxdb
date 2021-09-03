#ifndef MATCHBOXDB_INSERT_STATEMENT_H
#define MATCHBOXDB_INSERT_STATEMENT_H

#include <memory>
#include <vector>

#include "statement.h"
#include "table.h"

namespace ast {

class Expression;

class InsertStatement : public Statement {
public:
    InsertStatement(Table table, std::vector<std::unique_ptr<Expression>> inserts);

    [[nodiscard]] const Table &table() const;

    [[nodiscard]] const std::vector<std::unique_ptr<Expression>> &inserts() const;

    void repr(std::ostream &os) const override;

    void accept(StatementVisitor &visitor) const override;

protected:
    Table _table;
    std::vector<std::unique_ptr<Expression>> _inserts;
};

}

#endif //MATCHBOXDB_INSERT_STATEMENT_H
