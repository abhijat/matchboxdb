#include "string_literal.h"
#include "expression_visitor.h"

#include <utility>

ast::StringLiteral::StringLiteral(std::string value) : _value(std::move(value)) {}

const std::string &ast::StringLiteral::value() const {
    return _value;
}

void ast::StringLiteral::repr(std::ostream &os) const {
    os << '"' << _value << '"';
}

std::optional<metadata::DataType> ast::StringLiteral::evaluate() const {
    return {_value};
}

std::optional<metadata::DataType> ast::StringLiteral::accept(ast::ExpressionVisitor &expression_visitor) const {
    return expression_visitor.visit(*this);
}
