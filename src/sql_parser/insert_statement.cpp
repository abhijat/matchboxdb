#include <sstream>

#include "expression.h"
#include "insert_statement.h"

ast::InsertStatement::InsertStatement(Table table, std::vector<std::unique_ptr<Expression>> inserts)
    : _table(std::move(table)), _inserts(std::move(inserts)) {}

const ast::Table &ast::InsertStatement::table() const {
    return _table;
}

const std::vector<std::unique_ptr<ast::Expression>> &ast::InsertStatement::inserts() const {
    return _inserts;
}

void ast::InsertStatement::repr(std::ostream &os) const {
    os << "INSERT INTO [";
    _table.repr(os);

    os << "] VALUES {";

    std::stringstream ss;
    for (const auto &insert: _inserts) {
        insert->repr(ss);
        ss << ", ";
    }

    auto s = ss.str();
    os << s.substr(0, s.size() - 2);

    os << "}";
}
