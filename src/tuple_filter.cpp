#include "tuple_filter.h"

#include "storage/tuple.h"
#include "sql_parser/expression.h"
#include "storage/metadata.h"
#include "sql_parser/expression_visitor.h"

bool
tuple_filter::matches(
    const tuple::Tuple &tuple,
    const metadata::Metadata &metadata,
    const ast::Expression *where
) {
    if (!where) {
        return true;
    }

    ast::EvaluatingExpressionVisitor v{tuple, metadata};
    auto result = v.evaluate(*where);

    if (!result) {
        // TODO define exception here
        throw std::invalid_argument("failed to eval expression for tuple");
    }

    // A where clause must necessarily eval to a boolean value
    try {
        return std::get<bool>(*result);
    } catch (const std::bad_variant_access &) {
        // TODO define exception here
        throw std::invalid_argument("SELECT where clause not evaluating to boolean");
    }
}
