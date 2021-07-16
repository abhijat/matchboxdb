#include <gtest/gtest.h>
#include "../../src/sql_parser/token.h"
#include "../../src/sql_parser/lexer.h"


TEST(Lexer, BasicTokens) {
    std::string input{",=():;"};
    std::vector<std::pair<token::Kind, std::string>> expected{
        {token::Kind::Comma,     ","},
        {token::Kind::Equals,    "="},
        {token::Kind::LParen,    "("},
        {token::Kind::RParen,    ")"},
        {token::Kind::Colon,     ":"},
        {token::Kind::Semicolon, ";"},
    };

    lexer::Lexer l{input};
    for (const auto&[kind, literal] : expected) {
        auto n = l.next_token();
        ASSERT_EQ(n.literal(), literal);
        ASSERT_EQ(n.kind(), kind);
    }
}