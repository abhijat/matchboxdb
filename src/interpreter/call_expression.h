#ifndef MATCHBOXDB_CALL_EXPRESSION_H
#define MATCHBOXDB_CALL_EXPRESSION_H

#include <memory>
#include <vector>

#include "expression.h"
#include "token.h"

namespace ast {

class CallExpression : public Expression {
public:
    CallExpression(token::Token token, std::unique_ptr<Expression> callable,
                   std::vector<std::unique_ptr<Expression>> arguments);

    [[nodiscard]] const std::unique_ptr<Expression> &callable() const;

    [[nodiscard]] const std::vector<std::unique_ptr<Expression>> &arguments() const;

    std::string token_literal() const override;

    std::ostream &repr(std::ostream &os) const override;

protected:
    token::Token _token;
    std::unique_ptr<Expression> _callable;
    std::vector<std::unique_ptr<Expression>> _arguments;
};

}


#endif //MATCHBOXDB_CALL_EXPRESSION_H
