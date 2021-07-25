#include "integer_literal.h"

#include <utility>

ast::IntegerLiteral::IntegerLiteral(int64_t value) : _value(value) {}

void ast::IntegerLiteral::repr(std::ostream &os) const {
    os << _value;
}

int64_t ast::IntegerLiteral::value() const {
    return _value;
}

