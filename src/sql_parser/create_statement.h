#ifndef MATCHBOXDB_CREATE_STATEMENT_H
#define MATCHBOXDB_CREATE_STATEMENT_H

#include <vector>
#include "statement.h"
#include "table.h"

namespace ast {

class FieldDefinition;

class CreateStatement : public Statement {
public:
    CreateStatement(Table table_name, std::vector<ast::FieldDefinition> field_definitions);

    [[nodiscard]] const Table &table_name() const;

    [[nodiscard]] const std::vector<ast::FieldDefinition> &field_definitions() const;

    void repr(std::ostream &os) const override;

protected:
    ast::Table _table_name;
    std::vector<ast::FieldDefinition> _field_definitions;
};
}

#endif //MATCHBOXDB_CREATE_STATEMENT_H
