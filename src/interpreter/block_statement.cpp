#include "block_statement.h"

#include <utility>

#include "object.h"

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
    os << "{\n";
    for (const auto &statement : _statements) {
        statement->repr(os);
        os << "\n";
    }
    os << "}\n";

    return os;
}

const std::vector<std::unique_ptr<ast::Statement>> &ast::BlockStatement::statements() const {
    return _statements;
}

std::unique_ptr<objects::Object> ast::BlockStatement::visit(eval::Visitor &visitor) const {
    return nullptr;
}
