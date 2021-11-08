#include <sstream>

#include "parse_errors.h"

const char *parser::UnexpectedToken::what() const noexcept {
    return _what.c_str();
}

parser::UnexpectedToken::UnexpectedToken(
    const std::string &arg,
    token::Kind expected,
    token::Kind actual
) : invalid_argument(arg), _expected(expected),
    _actual(actual), _what{arg} {
}

parser::UnexpectedToken
parser::UnexpectedToken::unexpected_token(const token::Kind &expected, const token::Kind &actual) {
    std::stringstream ss;
    ss << "expected: " << expected << ", found " << actual;
    return UnexpectedToken{ss.str(), expected, actual};
}

parser::ParserError::ParserError(const std::string &what) : invalid_argument(what), _what(what) {}

const char *parser::ParserError::what() const noexcept {
    return _what.c_str();
}
