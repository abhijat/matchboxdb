#ifndef MATCHBOXDB_STRING_LITERAL_H
#define MATCHBOXDB_STRING_LITERAL_H

#include "expression.h"

namespace ast {

class StringLiteral : public Expression {
public:
    explicit StringLiteral(std::string value);

    [[nodiscard]] const std::string &value() const;

    void repr(std::ostream &os) const override;

    std::optional<metadata::DataType> evaluate() const override;

    std::optional<metadata::DataType> accept(ExpressionVisitor &expression_visitor) const override;

protected:
    std::string _value{};
};
}

#endif //MATCHBOXDB_STRING_LITERAL_H
