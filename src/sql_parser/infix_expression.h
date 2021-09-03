#ifndef MATCHBOXDB_INFIX_EXPRESSION_H
#define MATCHBOXDB_INFIX_EXPRESSION_H

#include <memory>
#include "expression.h"
#include "token.h"

namespace ast {
class InfixExpression : public Expression {
public:
    InfixExpression(token::Token token, std::unique_ptr<Expression> left,
                    std::unique_ptr<Expression> right, std::string infix_operator);

    [[nodiscard]] const Expression *left() const;

    [[nodiscard]] const Expression *right() const;

    [[nodiscard]] const std::string &infix_operator() const;

    void repr(std::ostream &os) const override;

    std::optional<metadata::DataType> evaluate() const override;

    std::optional<metadata::DataType> accept(ExpressionVisitor &expression_visitor) const override;

protected:
    token::Token _token;
    std::unique_ptr<ast::Expression> _left;
    std::unique_ptr<ast::Expression> _right;
    std::string _infix_operator;
};
}

#endif //MATCHBOXDB_INFIX_EXPRESSION_H
