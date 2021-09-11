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
    Insert,
    Into,
    Table,
    Values,
    Where,
    From,
    And,
    Or,
    Set,
    Colon,
    Equals,
    DoubleQuotes,
    Bang,
    LT,
    GT,
    True,
    False,
    NE,
    ColumnKindString,
    ColumnKindUnsignedInteger,
    ColumnKindBoolean,
    String,
    Drop,
};

std::ostream &operator<<(std::ostream &os, Kind kind);

static std::map<std::string, Kind> k_keywords{
    {"SELECT",       Kind::Select},
    {"UPDATE",       Kind::Update},
    {"DELETE",       Kind::Delete},
    {"CREATE",       Kind::Create},
    {"INSERT",       Kind::Insert},
    {"INTO",         Kind::Into},
    {"TABLE",        Kind::Table},
    {"VALUES",       Kind::Values},
    {"WHERE",        Kind::Where},
    {"FROM",         Kind::From},
    {"AND",          Kind::And},
    {"OR",           Kind::Or},
    {"true",         Kind::True},
    {"false",        Kind::False},
    {"STRING",       Kind::ColumnKindString},
    {"UNSIGNED_INT", Kind::ColumnKindUnsignedInteger},
    {"BOOLEAN",      Kind::ColumnKindBoolean},
    {"SET",          Kind::Set},
    {"DROP",         Kind::Drop},
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
