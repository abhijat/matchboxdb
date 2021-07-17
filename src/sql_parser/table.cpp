#include "table.h"

#include <utility>

void ast::Table::repr(std::ostream &os) const {
    os << _table_name;
}

ast::Table::Table(std::string table_name) : _table_name(std::move(table_name)) {}
