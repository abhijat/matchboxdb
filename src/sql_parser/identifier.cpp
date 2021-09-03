#include "identifier.h"
#include "expression_visitor.h"

#include <utility>

ast::Identifier::Identifier(token::Token token, std::string value) : _token(std::move(token)),
                                                                     _value(std::move(value)) {}

const std::string &ast::Identifier::value() const {
    return _value;
}

void ast::Identifier::repr(std::ostream &os) const {
    os << _value;
}

const token::Token &ast::Identifier::token() const {
    return _token;
}

std::optional<metadata::DataType> ast::Identifier::evaluate() const {
    return {};
}

std::optional<metadata::DataType> ast::Identifier::accept(ast::ExpressionVisitor &expression_visitor) const {
    return expression_visitor.visit(*this);
}

bool ast::operator<(const ast::Identifier &lhs, const ast::Identifier &rhs) {
    return lhs.value() < rhs.value();
}
