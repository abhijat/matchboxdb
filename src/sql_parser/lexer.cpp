#include "lexer.h"

#include <utility>
#include <functional>

lexer::Lexer::Lexer(std::string input) : _input(std::move(input)) {
    read_character();
}

void lexer::Lexer::read_character() {
    if (_read_position >= _input.size()) {
        _character = 0;
    } else {
        _character = _input[_read_position];
    }

    _position = _read_position;
    _read_position += 1;
}

token::Token lexer::Lexer::next_token() {
    slurp_ws();

    token::Kind kind;
    std::string literal(1, static_cast<char>(_character));

    switch (_character) {
        case ';':
            kind = token::Kind::Semicolon;
            break;
        case '(':
            kind = token::Kind::LParen;
            break;
        case ')':
            kind = token::Kind::RParen;
            break;
        case ',':
            kind = token::Kind::Comma;
            break;
        case ':':
            kind = token::Kind::Colon;
            break;
        case '=':
            kind = token::Kind::Equals;
            break;
        case '"': {
            return read_string_literal();
        }
            break;
        case '!': {
            if (peek_character() == '=') {
                // we need to do another read_character to eat up the = which is used in this token.
                read_character();
                kind = token::Kind::NE;
                literal = "!=";
            } else {
                kind = token::Kind::Bang;
            }
        }
            break;
        case '<':
            kind = token::Kind::LT;
            break;
        case '>':
            kind = token::Kind::GT;
            break;
        case 0: {
            kind = token::Kind::EndOfInput;
            literal = "";
        }
            break;
        default: {
            if (std::isalpha(_character)) {
                return read_identifier();
            } else if (std::isdigit(_character)) {
                return read_number();
            } else {
                kind = token::Kind::Illegal;
            }
        }
    }

    read_character();
    return {kind, literal};
}

token::Token lexer::Lexer::read_identifier() {
    auto literal = read_token_while([](auto ch) {
        return std::isalnum(ch) || ch == '_';
    });
    auto kind = token::lookup_identifier(literal);
    return {kind, literal};
}

void lexer::Lexer::slurp_ws() {
    while (std::isspace(_character)) {
        read_character();
    }
}

std::string lexer::Lexer::read_token_while(const std::function<bool(unsigned char)> &predicate) {
    auto start = _position;

    while (predicate(_character)) {
        read_character();
    }

    return std::string(_input, start, _position - start);
}

token::Token lexer::Lexer::read_number() {
    auto literal = read_token_while([](auto ch) {
        return std::isdigit(ch);
    });
    return {token::Kind::Integer, literal};
}

unsigned char lexer::Lexer::peek_character() const {
    if (_position >= _input.size()) {
        return 0;
    } else {
        return _input[_read_position];
    }
}

token::Token lexer::Lexer::read_string_literal() {
    // advance over opening "
    read_character();

    auto literal = read_token_while([](auto ch) {
        return ch != '"';
    });

    if (_character != '"') {
        throw std::invalid_argument{"String does not close correctly: expected \" found " + std::string(1, _character)};
    }

    // advance over closing "
    read_character();

    return {token::Kind::String, literal};
}
