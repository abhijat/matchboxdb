#include "expression_statement.h"

#include <utility>

#include "eval.h"
#include "object.h"

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

const ast::Expression *ast::ExpressionStatement::expression() const {
    return _expression.get();
}

std::unique_ptr<objects::Object> ast::ExpressionStatement::visit(eval::Visitor &visitor) const {
    return visitor.visit(*this);
}
