#ifndef MATCHBOXDB_INTEGER_LITERAL_H
#define MATCHBOXDB_INTEGER_LITERAL_H

#include "token.h"
#include "expression.h"

namespace ast {
class IntegerLiteral : public Expression {

public:
    IntegerLiteral(int64_t value);

    void repr(std::ostream &os) const override;

    [[nodiscard]] int64_t value() const;

    std::optional<metadata::DataType> evaluate() const override;

    std::optional<metadata::DataType> accept(ExpressionVisitor &expression_visitor) const override;

protected:
    int64_t _value;
};
}

#endif //MATCHBOXDB_INTEGER_LITERAL_H
