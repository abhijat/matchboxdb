#include "expression_statement.h"

#include <utility>

void ast::ExpressionStatement::repr(std::ostream &os) const {
    os << _token.literal();
}

ast::ExpressionStatement::ExpressionStatement(token::Token token, std::unique_ptr<Expression> expression)
    : _token(std::move(token)), _expression(std::move(expression)) {}

const std::unique_ptr<ast::Expression> &ast::ExpressionStatement::expression() const {
    return _expression;
}

void ast::ExpressionStatement::accept(ast::StatementVisitor &visitor) const {

}
