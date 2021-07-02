#include "call_expression.h"

#include <utility>

#include "object.h"

ast::CallExpression::CallExpression(token::Token token, std::unique_ptr<Expression> callable,
                                    std::vector<std::unique_ptr<Expression>> arguments) : _token(std::move(token)),
                                                                                          _callable(
                                                                                              std::move(callable)),
                                                                                          _arguments(std::move(
                                                                                              arguments)) {}

const std::unique_ptr<ast::Expression> &ast::CallExpression::callable() const {
    return _callable;
}

const std::vector<std::unique_ptr<ast::Expression>> &ast::CallExpression::arguments() const {
    return _arguments;
}

std::string ast::CallExpression::token_literal() const {
    return _token.literal;
}

std::ostream &ast::CallExpression::repr(std::ostream &os) const {
    _callable->repr(os);
    os << "(";

    if (!_arguments.empty()) {
        auto i = _arguments.cbegin();
        while (i < _arguments.cend() - 1) {
            (*i)->repr(os);
            os << ", ";
            i += 1;
        }
        (*i)->repr(os);
    }

    os << ")";
    return os;
}

std::unique_ptr<objects::Object> ast::CallExpression::visit(eval::Visitor &visitor) const {
    return nullptr;
}
