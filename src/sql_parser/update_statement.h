#ifndef MATCHBOXDB_UPDATE_STATEMENT_H
#define MATCHBOXDB_UPDATE_STATEMENT_H

#include <memory>
#include <optional>
#include <map>
#include <vector>

#include "statement.h"
#include "table.h"
#include "identifier.h"

namespace ast {

class Expression;

class UpdateStatement : public Statement {
public:
    UpdateStatement(ast::Table table, std::map<Identifier, std::unique_ptr<Expression>> update,
                    std::optional<std::unique_ptr<Expression>> where,
                    std::vector<Identifier> insertion_order);

    [[nodiscard]] const Table &table() const;

    [[nodiscard]] const std::map<Identifier, std::unique_ptr<ast::Expression>> &update() const;

    [[nodiscard]] std::vector<std::pair<Identifier, Expression*>> field_updates() const;

    [[nodiscard]] const Expression *where() const;

    void repr(std::ostream &os) const override;

    void accept(StatementVisitor &visitor) const override;

protected:
    Table _table;
    std::map<Identifier, std::unique_ptr<Expression>> _update;
    std::vector<Identifier> _insertion_order;
    std::optional<std::unique_ptr<Expression>> _where;
};
}

#endif //MATCHBOXDB_UPDATE_STATEMENT_H
