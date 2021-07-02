#include "program.h"

#include <ostream>
#include <utility>

#include "eval.h"

std::string ast::Program::token_literal() const {
    if (!_statements.empty()) {
        return _statements[0]->token_literal();
    } else {
        return "";
    }
}

const std::vector<std::unique_ptr<ast::Statement>> &ast::Program::statements() const {
    return _statements;
}

ast::Program::Program(std::vector<std::unique_ptr<Statement>> statements) : _statements(std::move(statements)) {}

std::ostream &ast::Program::repr(std::ostream &os) const {
    for (const auto &statement : _statements) {
        statement->repr(os);
    }
    return os;
}

std::unique_ptr<objects::Object> ast::Program::visit(eval::Visitor &visitor) const {
    return visitor.visit(*this);
}
