#include "boolean_literal.h"
#include "expression_visitor.h"

#include <utility>

ast::BooleanLiteral::BooleanLiteral(bool value) : _value(value) {}

void ast::BooleanLiteral::repr(std::ostream &os) const {
    os << _value;
}

bool ast::BooleanLiteral::value() const {
    return _value;
}

std::optional<metadata::DataType> ast::BooleanLiteral::evaluate() const {
    return {_value};
}

std::optional<metadata::DataType> ast::BooleanLiteral::accept(ast::ExpressionVisitor &expression_visitor) const {
    return expression_visitor.visit(*this);
}
