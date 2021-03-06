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

    void repr(std::ostream &os) const override;

    [[nodiscard]] const std::unique_ptr<Expression> &expression() const;

    void accept(StatementVisitor &visitor) const override;

protected:
    token::Token _token;
    std::unique_ptr<Expression> _expression;
};
}

#endif //MATCHBOXDB_EXPRESSION_STATEMENT_H
