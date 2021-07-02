#include "boolean_expression.h"

#include <utility>

#include "eval.h"
#include "object.h"

ast::BooleanExpression::BooleanExpression(token::Token token, bool value) : _token(std::move(token)), _value(value) {}

bool ast::BooleanExpression::value() const {
    return _value;
}

std::string ast::BooleanExpression::token_literal() const {
    return _token.literal;
}

std::ostream &ast::BooleanExpression::repr(std::ostream &os) const {
    os << std::boolalpha << _value << std::noboolalpha;
    return os;
}

std::unique_ptr<objects::Object> ast::BooleanExpression::visit(eval::Visitor &visitor) const {
    return visitor.visit(*this);
}
