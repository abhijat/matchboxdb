#include "token.h"

#include <utility>

token::Kind token::Token::kind() const {
    return _kind;
}

const std::string &token::Token::literal() const {
    return _literal;
}

token::Token::Token(token::Kind kind, std::string literal) : _kind(kind), _literal(std::move(literal)) {}
