#include "integer_literal.h"
#include "expression_visitor.h"


ast::IntegerLiteral::IntegerLiteral(int64_t value) : _value(value) {}

void ast::IntegerLiteral::repr(std::ostream &os) const {
    os << _value;
}

int64_t ast::IntegerLiteral::value() const {
    return _value;
}

std::optional<metadata::DataType> ast::IntegerLiteral::evaluate() const {
    return std::optional<metadata::DataType>{uint32_t(_value)};
}

std::optional<metadata::DataType> ast::IntegerLiteral::accept(ast::ExpressionVisitor &expression_visitor) const {
    return expression_visitor.visit(*this);
}

