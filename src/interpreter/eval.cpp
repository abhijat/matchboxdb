#include "eval.h"

#include "object.h"
#include "boolean_expression.h"
#include "infix_expression.h"
#include "expression_statement.h"
#include "integer_literal.h"
#include "program.h"

std::unique_ptr<objects::Object> eval::Visitor::visit(const ast::Program &program) {
    auto statements = std::vector<const ast::Statement *>{};
    for (const auto &st : program.statements()) {
        statements.push_back(st.get());
    }

    return this->visit(statements);
}

std::unique_ptr<objects::Object> eval::Visitor::visit(const ast::IntegerLiteral &integer_literal) {
    return std::make_unique<objects::Integer>(integer_literal.value());
}

std::unique_ptr<objects::Object> eval::Visitor::visit(const ast::ExpressionStatement &expression_statement) {
    return expression_statement.expression()->visit(*this);
}

std::unique_ptr<objects::Object> eval::Visitor::visit(const std::vector<const ast::Statement *> &statements) {
    auto result = std::unique_ptr<objects::Object>{};
    for (const auto *statement : statements) {
        result = statement->visit(*this);
    }
    return result;
}

std::unique_ptr<objects::Object> eval::Visitor::visit(const ast::BooleanExpression &boolean_expression) {
    return std::make_unique<objects::Boolean>(boolean_expression.value());
}

std::unique_ptr<objects::Object> eval::evaluate(const ast::Node &node) {
    Visitor visitor{};
    return node.visit(visitor);
}
