#include "drop_statement.h"

#include "statement_visitor.h"

#include <utility>

ast::DropStatement::DropStatement(ast::Table table) : _table(std::move(table)) {}

std::string ast::DropStatement::table_name() const {
    return _table.table_name();
}

void ast::DropStatement::repr(std::ostream &os) const {
    os << "DROP TABLE ";
    _table.repr(os);
}

void ast::DropStatement::accept(ast::StatementVisitor &visitor) const {
    visitor.visit(*this);
}
