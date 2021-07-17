#include "boolean_literal.h"

#include <utility>

ast::BooleanLiteral::BooleanLiteral(token::Token token, bool value) : _token(std::move(token)), _value(value) {}

void ast::BooleanLiteral::repr(std::ostream &os) const {
    os << _value;
}

bool ast::BooleanLiteral::value() const {
    return _value;
}
