#include "let_statement.h"

#include <utility>

std::string ast::LetStatement::token_literal() const {
    return _token.literal;
}

const ast::Identifier &ast::LetStatement::name() const {
    return _name;
}

const ast::Expression *ast::LetStatement::value() const {
    return _value.get();
}

ast::LetStatement::LetStatement(token::Token token, ast::Identifier name,
                                std::unique_ptr<Expression> value) :
    _token(std::move(token)),
    _name(std::move(name)),
    _value(std::move(value)) {}

std::ostream &ast::LetStatement::repr(std::ostream &os) const {
    os << token_literal() << " ";

    _name.repr(os) << " = ";

    if (_value) {
        _value->repr(os);
    }

    os << ";";
    return os;
}
