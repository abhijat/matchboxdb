#include "block_statement.h"

#include <utility>

ast::BlockStatement::BlockStatement(
    token::Token token,
    std::vector<std::unique_ptr<Statement>> statements
) : _token(std::move(token)),
    _statements(std::move(
        statements)) {}

std::string ast::BlockStatement::token_literal() const {
    return _token.literal;
}

std::ostream &ast::BlockStatement::repr(std::ostream &os) const {
    for (const auto &statement : _statements) {
        statement->repr(os);
    }

    return os;
}

const std::vector<std::unique_ptr<ast::Statement>> &ast::BlockStatement::statements() const {
    return _statements;
}
