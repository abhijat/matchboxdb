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
    for (const auto&[kind, literal]: expected) {
        auto n = l.next_token();
        ASSERT_EQ(n.literal(), literal);
        ASSERT_EQ(n.kind(), kind);
    }
}

TEST(Lexer, Identifiers) {
    std::string input{
        "SELECT name FROM CREATE TABLE person; UPDATE person WHERE name_123 DELETE AND x OR y true false DROP DESCRIBE"};
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
        {token::Kind::True,       "true"},
        {token::Kind::False,      "false"},
        {token::Kind::Drop,       "DROP"},
        {token::Kind::Describe,   "DESCRIBE"},
    };

    lexer::Lexer l{input};
    for (const auto&[kind, literal]: expected) {
        auto n = l.next_token();
        ASSERT_EQ(n.literal(), literal);
        ASSERT_EQ(n.kind(), kind);
    }
}

TEST(Lexer, Integers) {
    std::string input{"x 123 0 95 432"};
    std::vector<std::pair<token::Kind, std::string>> expected{
        {token::Kind::Identifier, "x"},
        {token::Kind::Integer,    "123"},
        {token::Kind::Integer,    "0"},
        {token::Kind::Integer,    "95"},
        {token::Kind::Integer,    "432"},
    };

    lexer::Lexer l{input};
    for (const auto&[kind, literal]: expected) {
        auto n = l.next_token();
        ASSERT_EQ(n.literal(), literal);
        ASSERT_EQ(n.kind(), kind);
    }
}

TEST(Lexer, SpecialChars) {
    lexer::Lexer l{"<>!!="};
    std::vector<std::pair<token::Kind, std::string>> expected{
        {token::Kind::LT,   "<"},
        {token::Kind::GT,   ">"},
        {token::Kind::Bang, "!"},
        {token::Kind::NE,   "!="},
    };

    for (const auto&[kind, literal]: expected) {
        auto n = l.next_token();
        ASSERT_EQ(n.kind(), kind);
        ASSERT_EQ(n.literal(), literal);
    }
}

TEST(Lexer, Strings) {
    lexer::Lexer l{R"("foo" is "bar" and "123-bar" is ":?:?foo")"};

    std::vector<std::pair<token::Kind, std::string>> expected{
        {token::Kind::String,     "foo"},
        {token::Kind::Identifier, "is"},
        {token::Kind::String,     "bar"},
        {token::Kind::Identifier, "and"},
        {token::Kind::String,     "123-bar"},
        {token::Kind::Identifier, "is"},
        {token::Kind::String,     ":?:?foo"},
    };

    for (const auto&[kind, literal]: expected) {
        auto n = l.next_token();
        ASSERT_EQ(n.kind(), kind);
        ASSERT_EQ(n.literal(), literal);
    }
}
