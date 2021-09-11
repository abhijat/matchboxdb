#ifndef MATCHBOXDB_EXPRESSION_H
#define MATCHBOXDB_EXPRESSION_H

#include <optional>
#include "node.h"
#include "../storage/metadata.h"

namespace ast {

class ExpressionVisitor;

class Expression : public Node {
public:
    [[nodiscard]] virtual std::optional<metadata::DataType> accept(ExpressionVisitor &expression_visitor) const = 0;

    [[nodiscard]] virtual std::optional<metadata::DataType> evaluate() const = 0;
};
}

#endif //MATCHBOXDB_EXPRESSION_H
