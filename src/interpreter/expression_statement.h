#ifndef MATCHBOXDB_EXPRESSION_STATEMENT_H
#define MATCHBOXDB_EXPRESSION_STATEMENT_H

#include <memory>

#include "statement.h"
#include "token.h"
#include "expression.h"

namespace ast {

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(token::Token token, std::unique_ptr<Expression> expression);

    [[nodiscard]] const Expression * expression() const;

    [[nodiscard]] std::string token_literal() const override;

    std::ostream &repr(std::ostream &os) const override;

    std::unique_ptr<objects::Object> visit(eval::Visitor &visitor) const override;

protected:
    token::Token _token;
    std::unique_ptr<Expression> _expression;
};

}


#endif //MATCHBOXDB_EXPRESSION_STATEMENT_H
