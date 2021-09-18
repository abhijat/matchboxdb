#include "describe_statement.h"

#include "statement_visitor.h"

#include <utility>

ast::DescribeStatement::DescribeStatement(std::optional<Table> table) : _table(std::move(table)) {}

const std::optional<ast::Table> &ast::DescribeStatement::table() const {
    return _table;
}

void ast::DescribeStatement::repr(std::ostream &os) const {
    os << "DESCRIBE TABLE";

    if (_table) {
        os << " ";
        _table->repr(os);
    } else {
        os << "S";
    }
}

void ast::DescribeStatement::accept(ast::StatementVisitor &visitor) const {
    visitor.visit(*this);
}
