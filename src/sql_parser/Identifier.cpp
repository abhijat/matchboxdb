#include "Identifier.h"

#include <utility>

ast::Identifier::Identifier(token::Token token, std::string value) : _token(std::move(token)),
                                                                     _value(std::move(value)) {}

const std::string &ast::Identifier::value() const {
    return _value;
}

void ast::Identifier::repr(std::ostream &os) const {
    os << "token: " << _token << ", value: " << _value;
}
