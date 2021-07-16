#ifndef MATCHBOXDB_TOKEN_H
#define MATCHBOXDB_TOKEN_H

#include <string>

namespace token {

enum class Kind {
    Illegal,
    EndOfInput,
    Indetifier,
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
    And,
    Or,
    Colon,
    Equals,
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

}

#endif //MATCHBOXDB_TOKEN_H
