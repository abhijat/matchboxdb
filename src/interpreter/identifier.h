#ifndef MATCHBOXDB_IDENTIFIER_H
#define MATCHBOXDB_IDENTIFIER_H

#include "expression.h"
#include "token.h"

namespace ast {

class Identifier: public Expression {
public:
    Identifier(token::Token token, std::string value);

    std::string token_literal() const override;

    const std::string &value() const;

protected:
    token::Token _token;
    std::string _value;
};

}


#endif //MATCHBOXDB_IDENTIFIER_H
