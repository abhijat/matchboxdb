#include "field_definition.h"

#include <utility>

ast::FieldDefinition::FieldDefinition(std::string field_name, token::Kind kind) : _field_name(std::move(field_name)) {
    auto pos = k_metadata_tokens.find(kind);
    if (pos == k_metadata_tokens.end()) {
        std::stringstream ss;
        ss << "bad column kind " << kind;
        throw std::invalid_argument{ss.str()};
    }
    _field_kind = pos->second;
}

const std::string &ast::FieldDefinition::field_name() const {
    return _field_name;
}

metadata::Kind ast::FieldDefinition::field_kind() const {
    return _field_kind;
}

void ast::FieldDefinition::repr(std::ostream &os) const {
    os << "{" << _field_name << ": " << _field_kind << "}";
}
