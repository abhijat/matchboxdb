#include "infix_expression.h"

#include <utility>

#include "eval.h"
#include "object.h"

ast::InfixExpression::InfixExpression(token::Token token, std::unique_ptr<Expression> left,
                                      std::unique_ptr<Expression> right, std::string infix_operator)
    : _token(std::move(token)), _left(std::move(left)), _right(std::move(right)),
      _infix_operator(std::move(infix_operator)) {}

const ast::Expression *ast::InfixExpression::left() const {
    return _left.get();
}

const ast::Expression *ast::InfixExpression::right() const {
    return _right.get();
}

const std::string &ast::InfixExpression::infix_operator() const {
    return _infix_operator;
}

std::string ast::InfixExpression::token_literal() const {
    return _token.literal;
}

std::ostream &ast::InfixExpression::repr(std::ostream &os) const {
    os << "(";
    _left->repr(os);
    os << " " << _infix_operator << " ";
    _right->repr(os);
    os << ")";
    return os;
}

std::unique_ptr<objects::Object> ast::InfixExpression::visit(eval::Visitor &visitor) const {
    return eval::Visitor::visit(*this);
}
