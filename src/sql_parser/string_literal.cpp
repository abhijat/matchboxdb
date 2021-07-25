#include "string_literal.h"

#include <utility>

ast::StringLiteral::StringLiteral(std::string value) : _value(std::move(value)) {}

const std::string &ast::StringLiteral::value() const {
    return _value;
}

void ast::StringLiteral::repr(std::ostream &os) const {
    os << '"' << _value << '"';
}
