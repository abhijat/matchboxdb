#include "token.h"

#include <utility>

token::Token::Token(token::TokenKind kind, std::string literal) : kind(kind), literal(std::move(literal)) {}

token::Token::Token() {}

bool token::Token::operator==(const token::Token &rhs) const {
    return kind == rhs.kind && literal == rhs.literal;
}

token::TokenKind token::lookup_identifier_kind(const std::string &identifier) {
    if (keywords.contains(identifier)) {
        return keywords.at(identifier);
    }

    return TokenKind::IDENT;
}

std::ostream &token::operator<<(std::ostream &os, const token::Token &t) {
    os << "{ Kind: " << t.kind << ", Literal: " << t.literal << " }";
    return os;
}

std::ostream &token::operator<<(std::ostream &os, token::TokenKind token_kind) {
    switch (token_kind) {
        case TokenKind::ILLEGAL:
            os << "ILLEGAL";
            break;
        case TokenKind::ENDOFINPUT:
            os << "ENDOFINPUT";
            break;
        case TokenKind::IDENT:
            os << "IDENT";
            break;
        case TokenKind::INT:
            os << "INT";
            break;
        case TokenKind::ASSIGN:
            os << "ASSIGN";
            break;
        case TokenKind::PLUS:
            os << "PLUS";
            break;
        case TokenKind::MINUS:
            os << "MINUS";
            break;
        case TokenKind::BANG:
            os << "BANG";
            break;
        case TokenKind::ASTERISK:
            os << "ASTERISK";
            break;
        case TokenKind::SLASH:
            os << "SLASH";
            break;
        case TokenKind::LT:
            os << "LT";
            break;
        case TokenKind::GT:
            os << "GT";
            break;
        case TokenKind::EQ:
            os << "EQ";
            break;
        case TokenKind::NE:
            os << "NE";
            break;
        case TokenKind::COMMA:
            os << "COMMA";
            break;
        case TokenKind::SEMICOLON:
            os << "SEMICOLON";
            break;
        case TokenKind::LPAREN:
            os << "LPAREN";
            break;
        case TokenKind::RPAREN:
            os << "RPAREN";
            break;
        case TokenKind::LBRACE:
            os << "LBRACE";
            break;
        case TokenKind::RBRACE:
            os << "RBRACE";
            break;
        case TokenKind::FUNCTION:
            os << "FUNCTION";
            break;
        case TokenKind::LET:
            os << "LET";
            break;
        case TokenKind::IF:
            os << "IF";
            break;
        case TokenKind::ELSE:
            os << "ELSE";
            break;
        case TokenKind::RETURN:
            os << "RETURN";
            break;
        case TokenKind::TRUE:
            os << "TRUE";
            break;
        case TokenKind::FALSE:
            os << "FALSE";
            break;
    }
    return os;
}
