#include "update_statement.h"

#include <utility>
#include <sstream>

#include "expression.h"

ast::UpdateStatement::UpdateStatement(ast::Table table, std::map<Identifier, std::unique_ptr<Expression>> update,
                                      std::optional<std::unique_ptr<Expression>> where,
                                      std::vector<Identifier> insertion_order
) : _table(std::move(table)),
    _update(std::move(update)),
    _where(std::move(where)),
    _insertion_order(std::move(insertion_order)) {}

const ast::Table &ast::UpdateStatement::table() const {
    return _table;
}

const std::map<ast::Identifier, std::unique_ptr<ast::Expression>> &ast::UpdateStatement::update() const {
    return _update;
}

const ast::Expression *ast::UpdateStatement::where() const {
    return _where->get();
}

void ast::UpdateStatement::repr(std::ostream &os) const {
    os << "UPDATE [";
    _table.repr(os);
    os << "] SET ";

    std::stringstream ss;

    ss << "{";
    for (const auto&[identifier, expression]: field_updates()) {
        identifier.repr(ss);
        ss << " = ";
        expression->repr(ss);
        ss << ", ";
    }

    auto s = ss.str();
    os << s.substr(0, s.size() - 2) << "}";

    if (_where) {
        os << " WHERE ...";
    }
}

std::vector<std::pair<ast::Identifier, ast::Expression *>> ast::UpdateStatement::field_updates() const {
    std::vector<std::pair<ast::Identifier, ast::Expression *>> updates;
    for (const auto &ordered: _insertion_order) {
        updates.emplace_back(ordered, _update.at(ordered).get());
    }
    return updates;
}
