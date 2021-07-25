#include "delete_statement.h"
#include "expression.h"

ast::DeleteStatement::DeleteStatement(ast::Table table, std::optional<std::unique_ptr<Expression>> where)
    : _table(std::move(table)), _where(std::move(where)) {

}

const ast::Table &ast::DeleteStatement::table() const {
    return _table;
}

const std::optional<std::unique_ptr<ast::Expression>> &ast::DeleteStatement::where() const {
    return _where;
}

void ast::DeleteStatement::repr(std::ostream &os) const {
    os << "DELETE FROM [";
    _table.repr(os);
    os << "]";

    if (_where) {
        os << " WHERE ";
        (*_where)->repr(os);
    }
}
