#ifndef MATCHBOXDB_PROGRAM_H
#define MATCHBOXDB_PROGRAM_H

#include <string>
#include <vector>
#include <memory>

#include "node.h"
#include "statement.h"

namespace ast {

class Program : public Node {
public:
    explicit Program(std::vector<std::unique_ptr<Statement>> statements);

    [[nodiscard]] std::string token_literal() const override;

    [[nodiscard]] const std::vector<std::unique_ptr<Statement>> &statements() const;

    std::ostream &repr(std::ostream &os) const override;

protected:
    std::vector<std::unique_ptr<Statement>> _statements;
};

}


#endif //MATCHBOXDB_PROGRAM_H
