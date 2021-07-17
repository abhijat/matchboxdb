#ifndef MATCHBOXDB_TOKEN_H
#define MATCHBOXDB_TOKEN_H

#include <string>
#include <map>

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
};

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

token::Kind lookup_identifier(const std::string &literal);

}

#endif //MATCHBOXDB_TOKEN_H
