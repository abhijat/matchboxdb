#include "expression_visitor.h"

#include "expression.h"
#include "boolean_literal.h"
#include "string_literal.h"
#include "integer_literal.h"
#include "infix_expression.h"
#include "identifier.h"

#include <utility>

ast::EvaluatingExpressionVisitor::EvaluatingExpressionVisitor(tuple::Tuple tuple, metadata::Metadata metadata)
    : _tuple(std::move(tuple)), _metadata(std::move(metadata)) {

    if (_tuple.attributes().size() != _metadata.names.size()) {
        // TODO define exception for this
        throw std::invalid_argument("tuple and metadata size do not match");
    }

    for (auto i = 0; i < _metadata.names.size(); ++i) {
        _tuple_as_map[_metadata.names[i]] = _tuple.attributes()[i];
    }
}

ast::EvaluationResult ast::EvaluatingExpressionVisitor::evaluate(const ast::Expression &expression) {
    return expression.accept(*this);
}

ast::EvaluationResult ast::EvaluatingExpressionVisitor::visit(const ast::Expression &expression) {
    return {};
}

ast::EvaluationResult ast::EvaluatingExpressionVisitor::visit(const ast::BooleanLiteral &boolean_literal) {
    return {boolean_literal.value()};
}

ast::EvaluationResult ast::EvaluatingExpressionVisitor::visit(const ast::StringLiteral &string_literal) {
    return {string_literal.value()};
}

ast::EvaluationResult ast::EvaluatingExpressionVisitor::visit(const ast::IntegerLiteral &integer_literal) {
    return {uint32_t(integer_literal.value())};
}

ast::EvaluationResult ast::EvaluatingExpressionVisitor::visit(const ast::InfixExpression &infix_expression) {
    auto lhs = infix_expression.left();
    auto rhs = infix_expression.right();

    auto lhs_evaluation = lhs->accept(*this);
    auto rhs_evaluation = rhs->accept(*this);

    if (!lhs_evaluation || !rhs_evaluation) {
        throw std::runtime_error("Failed to evaluate infix expression");
    }

    if (lhs_evaluation->index() != rhs_evaluation->index()) {
        // TODO define exception for this
        throw std::invalid_argument("cannot evaluate expression with unmatched types");
    }

    return evaluate_infix_expression(*lhs_evaluation, *rhs_evaluation, infix_expression.infix_operator());
}

ast::EvaluationResult ast::EvaluatingExpressionVisitor::visit(const ast::Identifier &identifier) {
    const auto &field_name = identifier.value();

    auto value = _tuple_as_map.find(field_name);
    if (value == _tuple_as_map.end()) {
        // TODO define exception for this
        throw std::invalid_argument("field name missing in row: " + field_name);
    }

    return value->second;
}

ast::EvaluatingExpressionVisitor ast::EvaluatingExpressionVisitor::context_less_evaluating_expression_visitor() {
    metadata::Metadata m{{},
                         {}};
    tuple::Tuple t{{}, m};
    return EvaluatingExpressionVisitor{t, m};
}

ast::EvaluationResult ast::evaluate_infix_expression(
    const metadata::DataType &lhs,
    const metadata::DataType &rhs,
    const std::string &op
) {
    BinaryOperationVisitor binary_operation_visitor{op, lhs};
    return std::visit(binary_operation_visitor, rhs);
}

ast::BinaryOperationVisitor::BinaryOperationVisitor(std::string op, metadata::DataType lhs)
    : _op(std::move(op)), _lhs(std::move(lhs)) {}

metadata::DataType ast::BinaryOperationVisitor::operator()(const std::string &rhs) const {
    auto lhs = std::get<std::string>(_lhs);
    if (_op == "=") {
        return lhs == rhs;
    } else if (_op == "!=") {
        return lhs != rhs;
    } else {
        throw std::invalid_argument("operator not supported for string types: " + _op);
    }
}

metadata::DataType ast::BinaryOperationVisitor::operator()(const uint32_t &rhs) const {
    auto lhs = std::get<uint32_t>(_lhs);

    if (_op == "=") {
        return lhs == rhs;
    } else if (_op == "!=") {
        return lhs != rhs;
    } else if (_op == "+") {
        return lhs + rhs;
    } else if (_op == "-") {
        return lhs - rhs;
    } else if (_op == ">") {
        return lhs > rhs;
    } else if (_op == "<") {
        return lhs < rhs;
    } else if (_op == ">=") {
        return lhs >= rhs;
    } else if (_op == "<=") {
        return lhs <= rhs;
    } else {
        throw std::invalid_argument("operator not supported for uint32_t types: " + _op);
    }
}

metadata::DataType ast::BinaryOperationVisitor::operator()(const bool &rhs) const {
    auto lhs = std::get<bool>(_lhs);

    if (_op == "=") {
        return lhs == rhs;
    } else if (_op == "!=") {
        return lhs != rhs;
    } else if (_op == "AND") {
        return lhs && rhs;
    } else if (_op == "OR") {
        return lhs || rhs;
    } else {
        throw std::invalid_argument("operator not supported for bool types: " + _op);
    }
}
