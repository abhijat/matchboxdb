#include "create_statement.h"
#include "field_definition.h"

#include <utility>

ast::CreateStatement::CreateStatement(
    ast::Table table_name,
    std::vector<ast::FieldDefinition> field_definitions
) : _table(std::move(table_name)),
    _field_definitions(std::move(field_definitions)) {}

const ast::Table &ast::CreateStatement::table_name() const {
    return _table;
}

const std::vector<ast::FieldDefinition> &ast::CreateStatement::field_definitions() const {
    return _field_definitions;
}

void ast::CreateStatement::repr(std::ostream &os) const {
    os << "CREATE TABLE ";
    _table.repr(os);
    os << "( ";
    for (const auto &fd: _field_definitions) {
        fd.repr(os);
        os << " ";
    }
    os << ")";
}

metadata::Metadata ast::CreateStatement::metadata() const {
    std::vector<std::string> names(_field_definitions.size());
    std::vector<metadata::Kind> kinds(_field_definitions.size());

    std::transform(_field_definitions.cbegin(), _field_definitions.cend(), names.begin(), [](const auto &fd) {
        return fd.field_name();
    });

    std::transform(_field_definitions.cbegin(), _field_definitions.cend(), kinds.begin(), [](const auto &fd) {
        return fd.field_kind();
    });

    return {names, kinds};
}
