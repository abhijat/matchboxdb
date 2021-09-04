#include "select_statement.h"
#include "statement_visitor.h"

#include <utility>
#include <sstream>

ast::SelectStatement::SelectStatement(
    std::vector<std::unique_ptr<ast::Expression>> selected,
    std::optional<ast::Table> table,
    std::optional<std::unique_ptr<ast::Expression>> where
) : _selected(std::move(selected)),
    _table(std::move(table)),
    _where(std::move(where)) {}

void ast::SelectStatement::repr(std::ostream &os) const {
    os << "SELECT ";

    std::stringstream ss;
    for (const auto &s: _selected) {
        s->repr(ss);
        ss << ", ";
    }

    auto s = ss.str();
    os << "{" << s.substr(0, s.size() - 2) << "}";

    if (_table) {
        os << " FROM [" << *_table << "]";
    }

    if (_where) {
        os << " WHERE ";
        _where->get()->repr(os);
    }
}

void ast::SelectStatement::accept(ast::StatementVisitor &visitor) const {
    visitor.visit(*this);
}

const std::vector<std::unique_ptr<ast::Expression>> &ast::SelectStatement::selected() const {
    return _selected;
}

const std::optional<ast::Table> &ast::SelectStatement::table() const {
    return _table;
}

const std::optional<std::unique_ptr<ast::Expression>> &ast::SelectStatement::where() const {
    return _where;
}
