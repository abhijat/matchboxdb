#include "select_statement.h"

#include <utility>

ast::SelectStatement::SelectStatement(
    std::vector<std::unique_ptr<ast::Expression>> selected,
    std::optional<ast::Table> table,
    std::optional<std::unique_ptr<ast::Expression>> where
) : _selected(std::move(selected)),
    _table(std::move(table)),
    _where(std::move(where)) {}

void ast::SelectStatement::repr(std::ostream &os) const {
    os << "SELECT ";
    if (_table) {
        os << " FROM " << *_table;
    }

    if (_where) {
        os << " WHERE ";
    }
}
