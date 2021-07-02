#ifndef MATCHBOXDB_IDENTIFIER_H
#define MATCHBOXDB_IDENTIFIER_H

#include "expression.h"
#include "token.h"

namespace ast {

class Identifier: public Expression {
public:
    Identifier(token::Token token, std::string value);

    [[nodiscard]] std::string token_literal() const override;

    [[nodiscard]] const std::string &value() const;

    std::ostream &repr(std::ostream &os) const override;

    std::unique_ptr<objects::Object> visit(eval::Visitor &visitor) const override;

protected:
    token::Token _token;
    std::string _value;
};

}


#endif //MATCHBOXDB_IDENTIFIER_H
