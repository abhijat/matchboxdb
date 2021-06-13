#include <gtest/gtest.h>

#include "../../src/interpreter/lexer.h"

TEST(Lexer, SimpleCharacters) {
    std::string input{"=+(){},;"};
    std::vector<token::Token> expected{
        {token::TokenKind::ASSIGN,    "="},
        {token::TokenKind::PLUS,      "+"},
        {token::TokenKind::LPAREN,    "("},
        {token::TokenKind::RPAREN,    ")"},
        {token::TokenKind::LBRACE,    "{"},
        {token::TokenKind::RBRACE,    "}"},
        {token::TokenKind::COMMA,     ","},
        {token::TokenKind::SEMICOLON, ";"},
    };

    lexer::Lexer l{input};
    for (const auto &e: expected) {
        ASSERT_EQ(e, l.next_token());
    }
}