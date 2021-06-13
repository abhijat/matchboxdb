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
    Program(std::vector<std::unique_ptr<Statement>> statements);

    std::string token_literal() const override;

    const std::vector<std::unique_ptr<Statement>> &statements() const;

    std::ostream &repr(std::ostream &os) const override;

protected:
    std::vector<std::unique_ptr<Statement>> _statements;
};

std::ostream &operator<<(std::ostream &os, const Program &program);

}


#endif //MATCHBOXDB_PROGRAM_H
