#ifndef MATCHBOXDB_LEXER_H
#define MATCHBOXDB_LEXER_H

#include <string>
#include <functional>
#include "token.h"

namespace lexer {

class Lexer {
public:
    explicit Lexer(std::string input);

    token::Token next_token();

protected:
    void read_character();
    unsigned char peek_character() const;

protected:
    std::string _input;
    int _position{};
    int _read_position{};
    unsigned char _character{};

    token::Token read_identifier();

    void slurp_ws();

    std::string read_token_while(const std::function<bool(unsigned char)> &predicate);

    token::Token read_number();

    token::Token read_string_literal();
};

}

#endif //MATCHBOXDB_LEXER_H
