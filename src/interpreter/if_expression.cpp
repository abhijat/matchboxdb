#include "if_expression.h"

#include <utility>

#include "object.h"

ast::IfExpression::IfExpression(token::Token token, std::unique_ptr<ast::Expression> condition,
                                BlockStatement consequence, std::optional<BlockStatement> alternative) :
    _token(std::move(token)),
    _condition(std::move(condition)),
    _consequence(std::move(consequence)),
    _alternative(std::move(alternative)) {}

std::string ast::IfExpression::token_literal() const {
    return _token.literal;
}

std::ostream &ast::IfExpression::repr(std::ostream &os) const {
    os << "if (";
    _condition->repr(os);
    os << ") ";
    _consequence.repr(os);
    if (_alternative) {
        os << "else ";
        _alternative->repr(os);
    }
    return os;
}

const ast::Expression * ast::IfExpression::condition() const {
    return _condition.get();
}

const ast::BlockStatement &ast::IfExpression::consequence() const {
    return _consequence;
}

const std::optional<ast::BlockStatement> &ast::IfExpression::alternative() const {
    return _alternative;
}

std::unique_ptr<objects::Object> ast::IfExpression::visit(eval::Visitor &visitor) const {
    return nullptr;
}
