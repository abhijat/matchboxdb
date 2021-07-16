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

TEST(Lexer, Identifiers) {
    std::string input{"SELECT name FROM CREATE TABLE person; UPDATE person WHERE name_123 DELETE AND x OR y"};
    std::vector<std::pair<token::Kind, std::string>> expected{
        {token::Kind::Select,     "SELECT"},
        {token::Kind::Identifier, "name"},
        {token::Kind::From,       "FROM"},
        {token::Kind::Create,     "CREATE"},
        {token::Kind::Table,      "TABLE"},
        {token::Kind::Identifier, "person"},
        {token::Kind::Semicolon,  ";"},
        {token::Kind::Update,     "UPDATE"},
        {token::Kind::Identifier, "person"},
        {token::Kind::Where,      "WHERE"},
        {token::Kind::Identifier, "name_123"},
        {token::Kind::Delete,     "DELETE"},
        {token::Kind::And,        "AND"},
        {token::Kind::Identifier, "x"},
        {token::Kind::Or,         "OR"},
        {token::Kind::Identifier, "y"},
    };

    lexer::Lexer l{input};
    for (const auto&[kind, literal] : expected) {
        auto n = l.next_token();
        ASSERT_EQ(n.literal(), literal);
        ASSERT_EQ(n.kind(), kind);
    }
}

TEST(Lexer, IllegalIdentifier) {
    std::string input{"1_foobar"};
    lexer::Lexer l{input};

    ASSERT_EQ(l.next_token().kind(), token::Kind::Illegal);
}