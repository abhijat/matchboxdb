#include "function_expression.h"

#include <utility>

ast::FunctionExpression::FunctionExpression(token::Token token, std::vector<Identifier> parameters,
                                            ast::BlockStatement body) : _token(std::move(token)),
                                                                        _parameters(std::move(parameters)),
                                                                        _body(std::move(body)) {}

std::string ast::FunctionExpression::token_literal() const {
    return _token.literal;
}

std::ostream &ast::FunctionExpression::repr(std::ostream &os) const {
    os << token_literal();
    os << "(";

    auto i = _parameters.cbegin();
    while (i < _parameters.cend() - 1) {
        i->repr(os);
        os << ", ";
        i += 1;
    }
    i->repr(os);
    os << ")";

    _body.repr(os);
    return os;
}

const std::vector<ast::Identifier> &ast::FunctionExpression::parameters() const {
    return _parameters;
}

const ast::BlockStatement &ast::FunctionExpression::body() const {
    return _body;
}
