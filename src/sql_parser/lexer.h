#ifndef MATCHBOXDB_LEXER_H
#define MATCHBOXDB_LEXER_H

#include <string>
#include "token.h"

namespace lexer {

class Lexer {
public:
    explicit Lexer(std::string input);

    token::Token next_token();

protected:
    void read_character();

protected:
    std::string _input;
    int _position{};
    int _read_position{};
    unsigned char _character{};
};

}

#endif //MATCHBOXDB_LEXER_H
