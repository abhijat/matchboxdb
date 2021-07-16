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
        case 0: {
            kind = token::Kind::EndOfInput;
            literal = "";
        }
            break;
        default: {
            if (std::isalpha(_character)) {
                return read_identifier();
            } else {
                kind = token::Kind::Illegal;
            }
        }
    }

    read_character();
    return {kind, literal};
}

token::Token lexer::Lexer::read_identifier() {
    auto start = _position;

    // we know the first character is isalpha (that is how we reached here). anything after can be a number or _
    while (std::isalnum(_character) || _character == '_') {
        read_character();
    }

    std::string literal(_input, start, _position - start);
    auto kind = token::lookup_identifier(literal);
    return token::Token{kind, std::string(_input, start, _position - start)};
}

void lexer::Lexer::slurp_ws() {
    while (std::isspace(_character)) {
        read_character();
    }
}
