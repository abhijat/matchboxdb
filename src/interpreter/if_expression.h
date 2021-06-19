#ifndef MATCHBOXDB_IF_EXPRESSION_H
#define MATCHBOXDB_IF_EXPRESSION_H

#include <memory>
#include "expression.h"
#include "token.h"
#include "block_statement.h"

namespace ast {

class IfExpression : public Expression {
public:
    IfExpression(token::Token token, std::unique_ptr<ast::Expression> condition,
                 ast::BlockStatement consequence, std::optional<BlockStatement> alternative);

    [[nodiscard]] std::string token_literal() const override;

    std::ostream &repr(std::ostream &os) const override;

    [[nodiscard]] const Expression *condition() const;

    [[nodiscard]] const BlockStatement &consequence() const;

    [[nodiscard]] const std::optional<ast::BlockStatement> &alternative() const;

protected:
    token::Token _token;
    std::unique_ptr<ast::Expression> _condition;
    ast::BlockStatement _consequence;
    std::optional<ast::BlockStatement> _alternative;
};

}


#endif //MATCHBOXDB_IF_EXPRESSION_H
