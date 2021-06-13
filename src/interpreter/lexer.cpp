#include "lexer.h"

#include <utility>

lexer::Lexer::Lexer(std::string input) : input(std::move(input)), position{}, read_position{} {
    read_character();
}

void lexer::Lexer::read_character() {
    if (read_position >= input.size()) {
        character = 0;
    } else {
        character = input.at(read_position);
    }

    position = read_position;
    read_position += 1;
}

token::Token lexer::Lexer::next_token() {
    token::Token t;
    switch (character) {
        case '=':
            t = {token::TokenKind::ASSIGN, "="};
            break;
        case ';':
            t = {token::TokenKind::SEMICOLON, ";"};
            break;
        case '(':
            t = {token::TokenKind::LPAREN, "("};
            break;
        case ')':
            t = {token::TokenKind::RPAREN, ")"};
            break;
        case '{':
            t = {token::TokenKind::LBRACE, "{"};
            break;
        case '}':
            t = {token::TokenKind::RBRACE, "}"};
            break;
        case ',':
            t = {token::TokenKind::COMMA, ","};
            break;
        case '+':
            t = {token::TokenKind::PLUS, "+"};
            break;
        case 0:
            t = {token::TokenKind::ENDOFINPUT, ""};
            break;
    }

    read_character();
    return t;
}
