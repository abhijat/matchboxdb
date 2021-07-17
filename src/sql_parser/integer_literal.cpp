#include "integer_literal.h"

#include <utility>

ast::IntegerLiteral::IntegerLiteral(int64_t value, token::Token token) : _value(value), _token(std::move(token)) {}

void ast::IntegerLiteral::repr(std::ostream &os) const {
    os << _value;
}

int64_t ast::IntegerLiteral::value() const {
    return _value;
}

const token::Token &ast::IntegerLiteral::token1() const {
    return _token;
}
