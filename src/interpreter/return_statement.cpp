#include "return_statement.h"

#include <utility>

ast::ReturnStatement::ReturnStatement(token::Token token, std::unique_ptr<Expression> expression)
    : _token(std::move(token)), _expression(std::move(expression)) {}

std::string ast::ReturnStatement::token_literal() const {
    return _token.literal;
}
