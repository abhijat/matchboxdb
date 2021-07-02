#ifndef MATCHBOXDB_BLOCK_STATEMENT_H
#define MATCHBOXDB_BLOCK_STATEMENT_H

#include <vector>
#include <memory>
#include "statement.h"
#include "token.h"

namespace ast {

class BlockStatement : public Statement {
public:
    BlockStatement(token::Token token, std::vector<std::unique_ptr<Statement>> statements);

    [[nodiscard]] std::string token_literal() const override;

    std::ostream &repr(std::ostream &os) const override;

    [[nodiscard]] const std::vector<std::unique_ptr<Statement>> &statements() const;

    std::unique_ptr<objects::Object> visit(eval::Visitor &visitor) const override;

protected:
    token::Token _token;
    std::vector<std::unique_ptr<Statement>> _statements;
};

}


#endif //MATCHBOXDB_BLOCK_STATEMENT_H
