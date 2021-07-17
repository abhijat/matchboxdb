#ifndef MATCHBOXDB_INTEGER_LITERAL_H
#define MATCHBOXDB_INTEGER_LITERAL_H

#include "expression.h"
#include "token.h"

namespace ast {
class IntegerLiteral : public Expression {

public:
    IntegerLiteral(int64_t value, token::Token token);

    void repr(std::ostream &os) const override;

    int64_t value() const;

    const token::Token &token1() const;

protected:
    int64_t _value;
    token::Token _token;
};
}

#endif //MATCHBOXDB_INTEGER_LITERAL_H
