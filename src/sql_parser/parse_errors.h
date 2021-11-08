#ifndef MATCHBOXDB_PARSE_ERRORS_H
#define MATCHBOXDB_PARSE_ERRORS_H

#include <stdexcept>
#include "token.h"

namespace parser {

class UnexpectedToken : std::invalid_argument {
public:
    UnexpectedToken(const std::string &arg, token::Kind expected, token::Kind actual);

    static UnexpectedToken unexpected_token(const token::Kind &expected, const token::Kind &actual);

    [[nodiscard]] const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;

protected:
    std::string _what;
    token::Kind _expected;
    token::Kind _actual;
};

class ParserError : std::invalid_argument {
public:
    explicit ParserError(const std::string &what);

    [[nodiscard]] const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;

protected:
    std::string _what;
};

}

#endif //MATCHBOXDB_PARSE_ERRORS_H
