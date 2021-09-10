#ifndef MATCHBOXDB_EXPRESSION_VISITOR_H
#define MATCHBOXDB_EXPRESSION_VISITOR_H

#include <optional>
#include <unordered_map>
#include "../metadata.h"
#include "../tuple.h"

namespace ast {

class Expression;

class BooleanLiteral;

class StringLiteral;

class IntegerLiteral;

class InfixExpression;

class Identifier;

using EvaluationResult = std::optional<metadata::DataType>;

class ExpressionVisitor {

public:

    virtual EvaluationResult visit(const Expression &expression) = 0;

    virtual EvaluationResult visit(const BooleanLiteral &boolean_literal) = 0;

    virtual EvaluationResult visit(const StringLiteral &string_literal) = 0;

    virtual EvaluationResult visit(const IntegerLiteral &integer_literal) = 0;

    virtual EvaluationResult visit(const InfixExpression &infix_expression) = 0;

    virtual EvaluationResult visit(const Identifier &identifier) = 0;
};

class EvaluatingExpressionVisitor : public ExpressionVisitor {

public:
    explicit EvaluatingExpressionVisitor(tuple::Tuple tuple, metadata::Metadata metadata);

    static EvaluatingExpressionVisitor context_less_evaluating_expression_visitor();

    EvaluationResult evaluate(const Expression &expression);

    EvaluationResult visit(const Expression &expression) override;

    EvaluationResult visit(const BooleanLiteral &boolean_literal) override;

    EvaluationResult visit(const StringLiteral &string_literal) override;

    EvaluationResult visit(const IntegerLiteral &integer_literal) override;

    EvaluationResult visit(const InfixExpression &infix_expression) override;

    EvaluationResult visit(const Identifier &identifier) override;

protected:
    tuple::Tuple _tuple;
    metadata::Metadata _metadata;
    std::unordered_map<std::string, metadata::DataType> _tuple_as_map;
};

EvaluationResult
evaluate_infix_expression(const metadata::DataType &lhs, const metadata::DataType &rhs, const std::string &op);

class BinaryOperationVisitor {
public:
    explicit BinaryOperationVisitor(std::string op, metadata::DataType lhs);

    metadata::DataType operator()(const std::string &rhs) const;

    metadata::DataType operator()(const uint32_t &rhs) const;

    metadata::DataType operator()(const bool &rhs) const;

protected:
    std::string _op;
    metadata::DataType _lhs;
};


}

#endif //MATCHBOXDB_EXPRESSION_VISITOR_H
