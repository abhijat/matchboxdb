#ifndef MATCHBOXDB_INTEGER_LITERAL_H
#define MATCHBOXDB_INTEGER_LITERAL_H

#include "expression.h"
#include "token.h"

namespace ast {

class IntegerLiteral : public Expression {
public:
    IntegerLiteral(token::Token token, int64_t value);

    [[nodiscard]] int64_t value() const;

    [[nodiscard]] std::string token_literal() const override;

    std::unique_ptr<objects::Object> visit(eval::Visitor &visitor) const override;

    std::ostream &repr(std::ostream &os) const override;

protected:
    token::Token _token;
    int64_t _value;
};

}

#endif //MATCHBOXDB_INTEGER_LITERAL_H
