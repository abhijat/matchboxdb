#ifndef MATCHBOXDB_PREFIX_EXPRESSION_H
#define MATCHBOXDB_PREFIX_EXPRESSION_H

#include <memory>
#include "expression.h"
#include "token.h"

namespace ast {

class PrefixExpression : public Expression {
public:
    PrefixExpression(token::Token token, std::string prefix_operator,
                     std::unique_ptr<Expression> right);

    [[nodiscard]] const std::string &prefix_operator() const;

    [[nodiscard]] const Expression *right() const;

    [[nodiscard]] std::string token_literal() const override;

    std::ostream &repr(std::ostream &os) const override;

    std::unique_ptr<objects::Object> visit(eval::Visitor &visitor) const override;

protected:
    token::Token _token;
    std::string _prefix_operator;
    std::unique_ptr<ast::Expression> _right;
};

}


#endif //MATCHBOXDB_PREFIX_EXPRESSION_H
