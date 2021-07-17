#ifndef MATCHBOXDB_TOKEN_H
#define MATCHBOXDB_TOKEN_H

#include <string>
#include <map>
#include <ostream>

namespace token {


enum class Kind {
    Illegal,
    EndOfInput,
    Identifier,
    Integer,
    Comma,
    Semicolon,
    LParen,
    RParen,
    Select,
    Update,
    Create,
    Delete,
    Table,
    Where,
    From,
    And,
    Or,
    Colon,
    Equals,
    DoubleQuotes,
    Bang,
    LT,
    GT,
    True,
    False,
    NE,
};

std::ostream &operator<<(std::ostream &os, Kind kind);

static std::map<std::string, Kind> k_keywords{
    {"SELECT", Kind::Select},
    {"UPDATE", Kind::Update},
    {"DELETE", Kind::Delete},
    {"CREATE", Kind::Create},
    {"TABLE",  Kind::Table},
    {"WHERE",  Kind::Where},
    {"FROM",   Kind::From},
    {"AND",    Kind::And},
    {"OR",     Kind::Or},
    {"true",   Kind::True},
    {"false",  Kind::False},
};

class Token {
public:
    Token(Kind kind, std::string literal);

    [[nodiscard]] Kind kind() const;

    [[nodiscard]] const std::string &literal() const;

protected:
    Kind _kind;
    std::string _literal;
};

std::ostream &operator<<(std::ostream &os, const Token &t);

token::Kind lookup_identifier(const std::string &literal);

}

#endif //MATCHBOXDB_TOKEN_H
