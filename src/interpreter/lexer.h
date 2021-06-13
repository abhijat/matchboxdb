#ifndef MATCHBOXDB_LEXER_H
#define MATCHBOXDB_LEXER_H

#include <string>
#include "token.h"

namespace lexer {

class Lexer {
public:
    Lexer(std::string input);

    void read_character();

    token::Token next_token();

    std::string read_identifier();

    std::string read_number();

    void eat_whitespace();

protected:
    std::string input;
    size_t read_position;
    size_t position;
    unsigned char character;
};

}

#endif //MATCHBOXDB_LEXER_H
