#include "integer_literal.h"

#include <utility>

ast::IntegerLiteral::IntegerLiteral(token::Token token, int64_t value) : _token(std::move(token)), _value(value) {}

int64_t ast::IntegerLiteral::value() const {
    return _value;
}

std::string ast::IntegerLiteral::token_literal() const {
    return _token.literal;
}

std::ostream &ast::IntegerLiteral::repr(std::ostream &os) const {
    os << _token.literal;
    return os;
}
