#ifndef MATCHBOXDB_LEXER_H
#define MATCHBOXDB_LEXER_H

#include <string>
#include "token.h"

namespace lexer {

class Lexer {
public:
    Lexer(std::string input);


    token::Token next_token();

protected:
    void read_character();

    std::string read_identifier();

    std::string read_number();

    void eat_whitespace();

    unsigned char peek_character() const;

protected:
    std::string input;
    size_t read_position;
    size_t position;
    unsigned char character;
};

}

#endif //MATCHBOXDB_LEXER_H
