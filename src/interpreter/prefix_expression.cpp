#include "prefix_expression.h"

#include <utility>

#include "object.h"

#include "eval.h"

ast::PrefixExpression::PrefixExpression(token::Token token, std::string prefix_operator,
                                        std::unique_ptr<Expression> right) : _token(std::move(token)),
                                                                             _prefix_operator(std::move(
                                                                                 prefix_operator)),
                                                                             _right(std::move(right)) {}

const std::string &ast::PrefixExpression::prefix_operator() const {
    return _prefix_operator;
}

const ast::Expression *ast::PrefixExpression::right() const {
    return _right.get();
}

std::string ast::PrefixExpression::token_literal() const {
    return _token.literal;
}

std::ostream &ast::PrefixExpression::repr(std::ostream &os) const {
    os << "(" << _prefix_operator;
    _right->repr(os);
    os << ")";
    return os;
}

std::unique_ptr<objects::Object> ast::PrefixExpression::visit(eval::Visitor &visitor) const {
    return visitor.visit(*this);
}
