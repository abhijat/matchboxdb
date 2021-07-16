#include "token.h"

#include <utility>

token::Kind token::Token::kind() const {
    return _kind;
}

const std::string &token::Token::literal() const {
    return _literal;
}

token::Token::Token(token::Kind kind, std::string literal) : _kind(kind), _literal(std::move(literal)) {}

token::Kind token::lookup_identifier(const std::string &literal) {
    auto search = k_keywords.find(literal);
    if (search == k_keywords.end()) {
        return Kind::Identifier;
    } else {
        return search->second;
    }
}
