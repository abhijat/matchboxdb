#include "eval.h"

#include "object.h"
#include "boolean_expression.h"
#include "expression_statement.h"
#include "integer_literal.h"
#include "program.h"
#include "prefix_expression.h"
#include "infix_expression.h"

eval::ObjectP eval::Visitor::visit(const ast::Program &program) {
    auto statements = std::vector<const ast::Statement *>{};
    for (const auto &st : program.statements()) {
        statements.push_back(st.get());
    }

    return this->visit(statements);
}

eval::ObjectP eval::Visitor::visit(const ast::IntegerLiteral &integer_literal) {
    return std::make_unique<objects::Integer>(integer_literal.value());
}

eval::ObjectP eval::Visitor::visit(const ast::ExpressionStatement &expression_statement) {
    return expression_statement.expression()->visit(*this);
}

eval::ObjectP eval::Visitor::visit(const std::vector<const ast::Statement *> &statements) {
    auto result = eval::ObjectP{};
    for (const auto *statement : statements) {
        result = statement->visit(*this);
    }
    return result;
}

eval::ObjectP eval::Visitor::visit(const ast::BooleanExpression &boolean_expression) {
    return std::make_unique<objects::Boolean>(boolean_expression.value());
}

eval::ObjectP eval::Visitor::visit(const ast::PrefixExpression &prefix_expression) {
    auto right_evaluated = prefix_expression.right()->visit(*this);
    return evaluate_prefix_expression(prefix_expression.prefix_operator(), std::move(right_evaluated));
}

eval::ObjectP eval::Visitor::evaluate_prefix_expression(const std::string &prefix_operator,
                                                        eval::ObjectP &&right_evaluated) {
    if (prefix_operator == "!") {
        return evaluate_bang_operator(std::move(right_evaluated));
    }

    if (prefix_operator == "-") {
        return evaluate_minus_prefix_operator(std::move(right_evaluated));
    }

    return nullptr;
}

eval::ObjectP eval::Visitor::evaluate_bang_operator(eval::ObjectP &&right_evaluated) {
    objects::BoolNegateVisitor visitor{};
    return right_evaluated->visit(visitor);
}

eval::ObjectP eval::Visitor::evaluate_minus_prefix_operator(eval::ObjectP &&right_evaluated) {
    objects::IntNegateVisitor visitor{};
    return right_evaluated->visit(visitor);
}

eval::ObjectP eval::Visitor::visit(const ast::InfixExpression &infix_expression) {
    return evaluate_infix_expression(
        infix_expression.infix_operator(),
        evaluate(*infix_expression.left()),
        evaluate(*infix_expression.right())
    );
}

eval::ObjectP
eval::Visitor::evaluate_infix_expression(const std::string &infix_operator, std::unique_ptr<objects::Object> left,
                                         std::unique_ptr<objects::Object> right) {
    if (left->kind() == objects::Kind::Integer && right->kind() == objects::Kind::Integer) {
        return evaluate_infix_expression(infix_operator, dynamic_cast<const objects::Integer *>(left.get()),
                                         dynamic_cast<const objects::Integer *>(right.get())
        );
    }
    return nullptr;
}

eval::ObjectP
eval::Visitor::evaluate_infix_expression(const std::string &infix_operator, const objects::Integer *left,
                                         const objects::Integer *right) {
    if (infix_operator == "+") {
        return std::make_unique<objects::Integer>(left->value() + right->value());
    } else if (infix_operator == "-") {
        return std::make_unique<objects::Integer>(left->value() - right->value());
    } else if (infix_operator == "*") {
        return std::make_unique<objects::Integer>(left->value() * right->value());
    } else if (infix_operator == "/") {
        return std::make_unique<objects::Integer>(left->value() / right->value());
    } else {
        return nullptr;
    }
}

eval::ObjectP eval::evaluate(const ast::Node &node) {
    Visitor visitor{};
    return node.visit(visitor);
}
