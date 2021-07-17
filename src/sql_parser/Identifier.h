#ifndef MATCHBOXDB_IDENTIFIER_H
#define MATCHBOXDB_IDENTIFIER_H

#include "expression.h"
#include "token.h"

namespace ast {
class Identifier : public Expression {
public:
    Identifier(token::Token token, std::string value);

    [[nodiscard]] const std::string &value() const;

private:
    void repr(std::ostream &os) const override;

protected:
    token::Token _token;
    std::string _value;
};
}

#endif //MATCHBOXDB_IDENTIFIER_H
