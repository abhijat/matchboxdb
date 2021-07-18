#ifndef MATCHBOXDB_IDENTIFIER_H
#define MATCHBOXDB_IDENTIFIER_H

#include "expression.h"
#include "token.h"

namespace ast {
class Identifier : public Expression {
public:
    Identifier(token::Token token, std::string value);

    [[nodiscard]] const std::string &value() const;

    [[nodiscard]] const token::Token &token() const;

    void repr(std::ostream &os) const override;


protected:
    token::Token _token;
    std::string _value;
};

bool operator<(const Identifier &lhs, const Identifier &rhs);

}

#endif //MATCHBOXDB_IDENTIFIER_H
