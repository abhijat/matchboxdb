#ifndef MATCHBOXDB_DELETE_STATEMENT_H
#define MATCHBOXDB_DELETE_STATEMENT_H

#include <vector>
#include <memory>
#include <optional>

#include "statement.h"
#include "table.h"

namespace ast {

class Expression;

class DeleteStatement : public Statement {
public:
    DeleteStatement(Table table, std::optional<std::unique_ptr<Expression>> where);

    [[nodiscard]] const Table &table() const;

    [[nodiscard]] const std::optional<std::unique_ptr<ast::Expression>> &where() const;

    void repr(std::ostream &os) const override;

protected:
    Table _table;
    std::optional<std::unique_ptr<Expression>> _where;
};

}

#endif //MATCHBOXDB_DELETE_STATEMENT_H
