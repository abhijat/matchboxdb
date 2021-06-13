#include "token.h"

#include <utility>

token::Token::Token(token::TokenKind kind, std::string literal) : kind(kind), literal(std::move(literal)) {}

token::Token::Token() {}

bool token::Token::operator==(const token::Token &rhs) const {
    return kind == rhs.kind && literal == rhs.literal;
}

token::TokenKind token::lookup_identifier_kind(const std::string &identifier) {
    if (keywords.contains(identifier)) {
        return keywords.at(identifier);
    }

    return TokenKind::IDENT;
}
