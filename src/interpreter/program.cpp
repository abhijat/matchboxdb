#include "program.h"

#include <utility>

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
