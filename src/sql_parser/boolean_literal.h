#ifndef MATCHBOXDB_BOOLEAN_LITERAL_H
#define MATCHBOXDB_BOOLEAN_LITERAL_H

#include "expression.h"
#include "token.h"

namespace ast {
class BooleanLiteral : public Expression {
public:
    BooleanLiteral(bool value);

    void repr(std::ostream &os) const override;

    [[nodiscard]] bool value() const;

protected:
    bool _value;
};
}

#endif //MATCHBOXDB_BOOLEAN_LITERAL_H
