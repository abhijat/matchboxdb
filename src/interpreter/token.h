#ifndef MATCHBOXDB_TOKEN_H
#define MATCHBOXDB_TOKEN_H

#include <string>
#include <map>

namespace token {

enum class TokenKind {
    ILLEGAL,
    ENDOFINPUT,
    IDENT,
    INT,
    ASSIGN,
    PLUS,
    COMMA,
    SEMICOLON,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    FUNCTION,
    LET,
};

static const std::map<std::string, TokenKind> keywords = {
    {"fn",  TokenKind::FUNCTION},
    {"let", TokenKind::LET},
};

TokenKind lookup_identifier_kind(const std::string& identifier);

struct Token {
    Token();

    Token(TokenKind kind, std::string literal);

    bool operator==(const Token &rhs) const;

    TokenKind kind;
    std::string literal;
};

}

#endif //MATCHBOXDB_TOKEN_H
