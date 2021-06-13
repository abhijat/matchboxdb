#include "expression_statement.h"

#include <utility>

ast::ExpressionStatement::ExpressionStatement(token::Token token, std::unique_ptr<Expression> expression)
    : _token(std::move(token)), _expression(std::move(expression)) {}

std::string ast::ExpressionStatement::token_literal() const {
    return _token.literal;
}

std::ostream &ast::ExpressionStatement::repr(std::ostream &os) const {
    if (_expression) {
        _expression->repr(os);
    }
    return os;
}
