#ifndef MATCHBOXDB_TOKEN_H
#define MATCHBOXDB_TOKEN_H

#include <iostream>
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
    MINUS,
    BANG,
    ASTERISK,
    SLASH,
    LT,
    GT,
    EQ,
    NE,

    COMMA,
    SEMICOLON,

    LPAREN,
    RPAREN,

    LBRACE,
    RBRACE,

    FUNCTION,
    LET,
    IF,
    ELSE,
    RETURN,
    TRUE,
    FALSE,
};

static const std::map<std::string, TokenKind> keywords = {
    {"fn",     TokenKind::FUNCTION},
    {"let",    TokenKind::LET},
    {"if",     TokenKind::IF},
    {"else",   TokenKind::ELSE},
    {"return", TokenKind::RETURN},
    {"true",   TokenKind::TRUE},
    {"false",  TokenKind::FALSE},
};

TokenKind lookup_identifier_kind(const std::string &identifier);

struct Token {
    Token();

    Token(TokenKind kind, std::string literal);

    bool operator==(const Token &rhs) const;

    TokenKind kind;
    std::string literal;
};

std::ostream &operator<<(std::ostream &os, const Token &t);

}

#endif //MATCHBOXDB_TOKEN_H
