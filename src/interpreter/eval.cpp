#include "eval.h"

#include "object.h"
#include "boolean_expression.h"
#include "expression_statement.h"
#include "integer_literal.h"
#include "program.h"
#include "prefix_expression.h"

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

std::unique_ptr<objects::Object> eval::Visitor::visit(const ast::PrefixExpression &prefix_expression) {
    auto right_evaluated = prefix_expression.right()->visit(*this);
    return evaluate_prefix_expression(prefix_expression.prefix_operator(), std::move(right_evaluated));
}

std::unique_ptr<objects::Object> eval::Visitor::evaluate_prefix_expression(const std::string &prefix_operator,
                                                                           std::unique_ptr<objects::Object> &&right_evaluated) {
    if (prefix_operator == "!") {
        return evaluate_bang_operator(std::move(right_evaluated));
    }

    if (prefix_operator == "-") {
        return evaluate_minus_prefix_operator(std::move(right_evaluated));
    }

    return nullptr;
}

std::unique_ptr<objects::Object>
eval::Visitor::evaluate_bang_operator(std::unique_ptr<objects::Object> &&right_evaluated) {
    // TODO remove dynamic cast here. use visitors?
    switch (right_evaluated->kind()) {
        case objects::Kind::Integer: {
            const auto i = dynamic_cast<const objects::Integer *>(right_evaluated.get());
            return std::make_unique<objects::Boolean>(i->value() == 0);
        }
        case objects::Kind::Boolean: {
            const auto b = dynamic_cast<const objects::Boolean *>(right_evaluated.get());
            return std::make_unique<objects::Boolean>(!b->value());
        }
        case objects::Kind::Null: {
            return std::make_unique<objects::Boolean>(true);
        }
    }

    return std::make_unique<objects::Boolean>(false);
}

std::unique_ptr<objects::Object>
eval::Visitor::evaluate_minus_prefix_operator(std::unique_ptr<objects::Object> &&right_evaluated) {
    switch (right_evaluated->kind()) {
        case objects::Kind::Integer: {
            const auto i = dynamic_cast<const objects::Integer *>(right_evaluated.get());
            return std::make_unique<objects::Integer>(-i->value());
        }
        default:
            return nullptr;
    }
}

std::unique_ptr<objects::Object> eval::evaluate(const ast::Node &node) {
    Visitor visitor{};
    return node.visit(visitor);
}
