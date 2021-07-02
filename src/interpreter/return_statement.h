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

    [[nodiscard]] std::string token_literal() const override;

    std::ostream &repr(std::ostream &os) const override;

    std::unique_ptr<objects::Object> visit(eval::Visitor &visitor) const override;

protected:
    token::Token _token;
    std::unique_ptr<Expression> _value;
};

}


#endif //MATCHBOXDB_RETURN_STATEMENT_H
