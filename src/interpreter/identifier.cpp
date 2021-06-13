#include "identifier.h"

#include <utility>

std::string ast::Identifier::token_literal() const {
    return _token.literal;
}

const std::string &ast::Identifier::value() const {
    return _value;
}

ast::Identifier::Identifier(token::Token token, std::string value) : _token(std::move(token)),
                                                                     _value(std::move(value)) {}

std::ostream &ast::Identifier::repr(std::ostream &os) const {
    os << _value;
    return os;
}
