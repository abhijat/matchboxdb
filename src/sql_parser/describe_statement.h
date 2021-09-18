#ifndef MATCHBOXDB_DESCRIBE_STATEMENT_H
#define MATCHBOXDB_DESCRIBE_STATEMENT_H

#include <optional>
#include <utility>

#include "statement.h"
#include "table.h"

namespace ast {

class DescribeStatement : public Statement {
public:
    explicit DescribeStatement(std::optional<Table> table);

    [[nodiscard]] const std::optional<Table> &table() const;

    void repr(std::ostream &os) const override;

    void accept(StatementVisitor &visitor) const override;

protected:
    std::optional<Table> _table;
};

}

#endif //MATCHBOXDB_DESCRIBE_STATEMENT_H
