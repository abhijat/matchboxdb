#ifndef MATCHBOXDB_EVAL_H
#define MATCHBOXDB_EVAL_H

#include <memory>
#include <vector>

namespace objects {
class Object;

class Boolean;
}

namespace ast {
class Program;

class BooleanExpression;

class IntegerLiteral;

class ExpressionStatement;

class Statement;

class Node;

class PrefixExpression;
}

namespace eval {

std::unique_ptr<objects::Object> evaluate(const ast::Node &node);

// TODO investigate if it would be better to just overload evaluate with specific arguments

class Visitor {
public:
    std::unique_ptr<objects::Object> visit(const ast::Program &program);

    static std::unique_ptr<objects::Object> visit(const ast::IntegerLiteral &integer_literal);

    static std::unique_ptr<objects::Object> visit(const ast::BooleanExpression &boolean_expression);

    std::unique_ptr<objects::Object> visit(const ast::ExpressionStatement &expression_statement);

    std::unique_ptr<objects::Object> visit(const std::vector<const ast::Statement *> &statements);

    std::unique_ptr<objects::Object> visit(const ast::PrefixExpression &prefix_expression);

    std::unique_ptr<objects::Object>
    evaluate_prefix_expression(const std::string &prefix_operator, std::unique_ptr<objects::Object> &&right_evaluated);

    std::unique_ptr<objects::Object> evaluate_bang_operator(std::unique_ptr<objects::Object> &&right_evaluated);

    std::unique_ptr<objects::Object> evaluate_minus_prefix_operator(std::unique_ptr<objects::Object> &&right_evaluated);
};

}

#endif //MATCHBOXDB_EVAL_H
