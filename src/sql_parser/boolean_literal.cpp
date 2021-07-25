#include "boolean_literal.h"

#include <utility>

ast::BooleanLiteral::BooleanLiteral(bool value) : _value(value) {}

void ast::BooleanLiteral::repr(std::ostream &os) const {
    os << _value;
}

bool ast::BooleanLiteral::value() const {
    return _value;
}
