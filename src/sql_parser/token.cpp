#include "token.h"

#include <utility>

token::Kind token::Token::kind() const {
    return _kind;
}

const std::string &token::Token::literal() const {
    return _literal;
}

token::Token::Token(token::Kind kind, std::string literal) : _kind(kind), _literal(std::move(literal)) {}

token::Kind token::lookup_identifier(const std::string &literal) {
    auto search = k_keywords.find(literal);
    if (search == k_keywords.end()) {
        return Kind::Identifier;
    } else {
        return search->second;
    }
}

std::ostream &token::operator<<(std::ostream &os, const token::Token &t) {
    os << "{ Kind: " << t.kind() << ", Literal: " << t.literal() << " }";
    return os;
}

std::ostream &token::operator<<(std::ostream &os, token::Kind kind) {
    switch (kind) {
        case Kind::Illegal:
            os << "Kind::Illegal";
            break;
        case Kind::EndOfInput:
            os << "Kind::EndOfInput";
            break;
        case Kind::Identifier:
            os << "Kind::Identifier";
            break;
        case Kind::Integer:
            os << "Kind::Integer";
            break;
        case Kind::Comma:
            os << "Kind::Comma";
            break;
        case Kind::Semicolon:
            os << "Kind::Semicolon";
            break;
        case Kind::LParen:
            os << "Kind::LParen";
            break;
        case Kind::RParen:
            os << "Kind::RParen";
            break;
        case Kind::Select:
            os << "Kind::Select";
            break;
        case Kind::Update:
            os << "Kind::Update";
            break;
        case Kind::Create:
            os << "Kind::Create";
            break;
        case Kind::Delete:
            os << "Kind::Delete";
            break;
        case Kind::Table:
            os << "Kind::Table";
            break;
        case Kind::Where:
            os << "Kind::Where";
            break;
        case Kind::From:
            os << "Kind::From";
            break;
        case Kind::And:
            os << "Kind::And";
            break;
        case Kind::Or:
            os << "Kind::Or";
            break;
        case Kind::Colon:
            os << "Kind::Colon";
            break;
        case Kind::Equals:
            os << "Kind::Equals";
            break;
        case Kind::DoubleQuotes:
            os << "Kind::DoubleQuotes";
            break;
        case Kind::Bang:
            os << "Kind::Bang";
            break;
        case Kind::LT:
            os << "Kind::LT";
            break;
        case Kind::GT:
            os << "Kind::GT";
            break;
        case Kind::True:
            os << "Kind::True";
            break;
        case Kind::False:
            os << "Kind::False";
            break;
        case Kind::NE:
            os << "Kind::NE";
        case Kind::String:
            os << "Kind::String";
            break;
        case Kind::Insert:
            os << "Kind::Insert";
            break;
        case Kind::Into:
            os << "Kind::Into";
            break;
        case Kind::Values:
            os << "Kind::Values";
            break;
        case Kind::Set:
            os << "Kind::Set";
            break;
        case Kind::ColumnKindString:
            os << "Kind::ColumnKindString";
            break;
        case Kind::ColumnKindUnsignedInteger:
            os << "Kind::ColumnKindUnsignedInteger";
            break;
        case Kind::ColumnKindBoolean:
            os << "Kind::ColumnKindBoolean";
            break;
    }
    return os;
}
