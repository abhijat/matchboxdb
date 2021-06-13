#ifndef MATCHBOXDB_RETURN_STATEMENT_H
#define MATCHBOXDB_RETURN_STATEMENT_H

#include <memory>

#include "statement.h"
#include "token.h"
#include "expression.h"

namespace ast {

class ReturnStatement : public Statement {
public:
    ReturnStatement(token::Token token, std::unique_ptr<Expression> expression);

    std::string token_literal() const override;

protected:
    token::Token _token;
    std::unique_ptr<Expression> _expression;
};

}


#endif //MATCHBOXDB_RETURN_STATEMENT_H
