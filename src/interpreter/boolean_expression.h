#ifndef MATCHBOXDB_BOOLEAN_EXPRESSION_H
#define MATCHBOXDB_BOOLEAN_EXPRESSION_H

#include "expression.h"
#include "token.h"

namespace ast {

class BooleanExpression : public Expression {
public:
    BooleanExpression(token::Token token, bool value);

    bool value() const;

    std::string token_literal() const override;

    std::ostream &repr(std::ostream &os) const override;

protected:
    token::Token _token;
    bool _value;
};

}


#endif //MATCHBOXDB_BOOLEAN_EXPRESSION_H
