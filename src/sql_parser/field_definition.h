#ifndef MATCHBOXDB_FIELD_DEFINITION_H
#define MATCHBOXDB_FIELD_DEFINITION_H

#include "node.h"
#include "../storage/metadata.h"
#include "token.h"

namespace {

static const std::map<token::Kind, metadata::Kind> k_metadata_tokens{
    {token::Kind::ColumnKindString,          metadata::Kind::String},
    {token::Kind::ColumnKindUnsignedInteger, metadata::Kind::UnsignedInt},
    {token::Kind::ColumnKindBoolean,         metadata::Kind::Boolean},
};

}

namespace ast {
class FieldDefinition : public Node {
public:
    FieldDefinition(std::string field_name, token::Kind kind);

    [[nodiscard]] const std::string &field_name() const;

    [[nodiscard]] metadata::Kind field_kind() const;

    void repr(std::ostream &os) const override;

protected:
    std::string _field_name;
    metadata::Kind _field_kind;
};
}


#endif //MATCHBOXDB_FIELD_DEFINITION_H
