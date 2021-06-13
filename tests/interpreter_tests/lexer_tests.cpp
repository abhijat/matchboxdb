#include <gtest/gtest.h>

#include "../../src/interpreter/lexer.h"

TEST(Lexer, SyntaxLexing) {
    std::string input{R"S(

    let five = 5;

    let ten = 10;

    let add = fn(x, y) {
        x + y;
    };

    let result = add(five, ten);

    !-/*5;
    5 < 10 > 5;

)S"};
    std::vector<token::Token> expected{
        {token::TokenKind::LET,       "let"},
        {token::TokenKind::IDENT,     "five"},
        {token::TokenKind::ASSIGN,    "="},
        {token::TokenKind::INT,       "5"},
        {token::TokenKind::SEMICOLON, ";"},

        {token::TokenKind::LET,       "let"},
        {token::TokenKind::IDENT,     "ten"},
        {token::TokenKind::ASSIGN,    "="},
        {token::TokenKind::INT,       "10"},
        {token::TokenKind::SEMICOLON, ";"},

        {token::TokenKind::LET,       "let"},
        {token::TokenKind::IDENT,     "add"},
        {token::TokenKind::ASSIGN,    "="},
        {token::TokenKind::FUNCTION,  "fn"},
        {token::TokenKind::LPAREN,    "("},
        {token::TokenKind::IDENT,     "x"},
        {token::TokenKind::COMMA,     ","},
        {token::TokenKind::IDENT,     "y"},
        {token::TokenKind::RPAREN,    ")"},
        {token::TokenKind::LBRACE,    "{"},
        {token::TokenKind::IDENT,     "x"},
        {token::TokenKind::PLUS,      "+"},
        {token::TokenKind::IDENT,     "y"},
        {token::TokenKind::SEMICOLON, ";"},
        {token::TokenKind::RBRACE,    "}"},
        {token::TokenKind::SEMICOLON, ";"},

        {token::TokenKind::LET,       "let"},
        {token::TokenKind::IDENT,     "result"},
        {token::TokenKind::ASSIGN,    "="},
        {token::TokenKind::IDENT,     "add"},
        {token::TokenKind::LPAREN,    "("},
        {token::TokenKind::IDENT,     "five"},
        {token::TokenKind::COMMA,     ","},
        {token::TokenKind::IDENT,     "ten"},
        {token::TokenKind::RPAREN,    ")"},
        {token::TokenKind::SEMICOLON, ";"},

        {token::TokenKind::BANG,      "!"},
        {token::TokenKind::MINUS,     "-"},
        {token::TokenKind::SLASH,     "/"},
        {token::TokenKind::ASTERISK,  "*"},
        {token::TokenKind::INT,       "5"},
        {token::TokenKind::SEMICOLON, ";"},

        {token::TokenKind::INT,       "5"},
        {token::TokenKind::LT,        "<"},
        {token::TokenKind::INT,       "10"},
        {token::TokenKind::GT,        ">"},
        {token::TokenKind::INT,       "5"},
        {token::TokenKind::SEMICOLON, ";"},
    };

    lexer::Lexer l{input};
    for (const auto &e: expected) {
        auto next_token = l.next_token();
        ASSERT_EQ(e, next_token);
    }
}
