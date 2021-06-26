#ifndef MATCHBOXDB_FUNCTION_EXPRESSION_H
#define MATCHBOXDB_FUNCTION_EXPRESSION_H

#include <vector>

#include "expression.h"
#include "token.h"
#include "identifier.h"
#include "block_statement.h"

namespace ast {

class FunctionExpression : public Expression {
public:
    FunctionExpression(token::Token token, std::vector<Identifier> parameters,
                       BlockStatement body);

    [[nodiscard]] std::string token_literal() const override;

    std::ostream &repr(std::ostream &os) const override;

    [[nodiscard]] const std::vector<Identifier> &parameters() const;

    [[nodiscard]] const BlockStatement &body() const;

protected:
    token::Token _token;
    std::vector<Identifier> _parameters;
    BlockStatement _body;
};

}


#endif //MATCHBOXDB_FUNCTION_EXPRESSION_H
