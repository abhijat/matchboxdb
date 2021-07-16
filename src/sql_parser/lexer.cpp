#include "lexer.h"

#include <utility>

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
        case 0: {
            kind = token::Kind::EndOfInput;
            literal = "";
        }
            break;
    }

    read_character();
    return {kind, literal};
}
