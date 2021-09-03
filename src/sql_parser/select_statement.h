#ifndef MATCHBOXDB_SELECT_STATEMENT_H
#define MATCHBOXDB_SELECT_STATEMENT_H

#include <vector>
#include <optional>
#include <memory>

#include "statement.h"
#include "table.h"
#include "expression.h"


namespace ast {

class SelectStatement : public Statement {
public:
    SelectStatement(std::vector<std::unique_ptr<ast::Expression>> selected,
                    std::optional<ast::Table> table,
                    std::optional<std::unique_ptr<ast::Expression>> where);

    void repr(std::ostream &os) const override;

    void accept(StatementVisitor &visitor) const override;

protected:
    // what we are selecting (e.g. the foo in SELECT foo)
    std::vector<std::unique_ptr<ast::Expression>> _selected;

    // where we are selecting it from. the table is not an expression, for now just one table at a time.
    // optional so we can do stuff like SELECT 1
    std::optional<ast::Table> _table;

    // optional filter expression
    std::optional<std::unique_ptr<ast::Expression>> _where;
};

}

#endif //MATCHBOXDB_SELECT_STATEMENT_H
