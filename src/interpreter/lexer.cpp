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
    eat_whitespace();

    switch (character) {
        case '=': {
            if (peek_character() == '=') {
                read_character();
                t = {token::TokenKind::EQ, "=="};
            } else {
                t = {token::TokenKind::ASSIGN, "="};
            }
        }
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
        case '!': {
            if (peek_character() == '=') {
                read_character();
                t = {token::TokenKind::NE, "!="};
            } else {
                t = {token::TokenKind::BANG, "!"};
            }
        }
            break;
        case '-':
            t = {token::TokenKind::MINUS, "-"};
            break;
        case '/':
            t = {token::TokenKind::SLASH, "/"};
            break;
        case '*':
            t = {token::TokenKind::ASTERISK, "*"};
            break;
        case '<':
            t = {token::TokenKind::LT, "<"};
            break;
        case '>':
            t = {token::TokenKind::GT, ">"};
            break;
        case 0:
            t = {token::TokenKind::ENDOFINPUT, ""};
            break;
        default: {
            if (std::isalpha(character)) {
                auto literal = read_identifier();
                auto kind = token::lookup_identifier_kind(literal);
                return {kind, literal};
            } else if (std::isdigit(character)) {
                return {token::TokenKind::INT, read_number()};
            } else {
                t = {token::TokenKind::ILLEGAL, std::string(1, character)};
            }
        }
    }

    read_character();
    return t;
}

std::string lexer::Lexer::read_identifier() {
    auto start = position;
    while (std::isalpha(character) || character == '_') {
        read_character();
    }

    return input.substr(start, position - start);
}

void lexer::Lexer::eat_whitespace() {
    while (std::isspace(character)) {
        read_character();
    }
}

std::string lexer::Lexer::read_number() {
    auto start = position;
    while (std::isdigit(character)) {
        read_character();
    }

    return input.substr(start, position - start);
}

unsigned char lexer::Lexer::peek_character() const {
    if (read_position >= input.size()) {
        return 0;
    } else {
        return input.at(read_position);
    }
}
