#ifndef MATCHBOXDB_LET_STATEMENT_H
#define MATCHBOXDB_LET_STATEMENT_H

#include <memory>

#include "identifier.h"
#include "statement.h"
#include "token.h"

namespace ast {

class LetStatement : public Statement {
public:
    LetStatement(token::Token token, Identifier name, std::unique_ptr<Expression> value);

    std::string token_literal() const override;

    const Identifier &name() const;

    const Expression *value() const;

    std::ostream &repr(std::ostream &os) const override;

protected:
    token::Token _token;
    Identifier _name;
    std::unique_ptr<Expression> _value;
};

}


#endif //MATCHBOXDB_LET_STATEMENT_H