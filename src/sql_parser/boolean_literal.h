#ifndef MATCHBOXDB_BOOLEAN_LITERAL_H
#define MATCHBOXDB_BOOLEAN_LITERAL_H

#include "expression.h"
#include "token.h"

namespace ast {
class BooleanLiteral : public Expression {
public:
    BooleanLiteral(token::Token token, bool value);

    void repr(std::ostream &os) const override;

    bool value() const;

protected:
    token::Token _token;
    bool _value;
};
}

#endif //MATCHBOXDB_BOOLEAN_LITERAL_H
