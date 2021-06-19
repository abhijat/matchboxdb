#include "return_statement.h"

#include <utility>

ast::ReturnStatement::ReturnStatement(token::Token token, std::unique_ptr<Expression> expression)
    : _token(std::move(token)), _value(std::move(expression)) {}

std::string ast::ReturnStatement::token_literal() const {
    return _token.literal;
}

std::ostream &ast::ReturnStatement::repr(std::ostream &os) const {
    os << token_literal() << " ";
    if (_value) {
        _value->repr(os);
    }

    os << ";";
    return os;
}