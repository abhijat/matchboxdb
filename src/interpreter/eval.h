#ifndef MATCHBOXDB_EVAL_H
#define MATCHBOXDB_EVAL_H

#include <memory>
#include <vector>
#include "expression.h"
#include "object.h"

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

class InfixExpression;
}

namespace eval {

using ObjectP = std::unique_ptr<objects::Object>;

std::unique_ptr<objects::Object> evaluate(const ast::Node &node);

// TODO investigate if it would be better to just overload evaluate with specific arguments

class Visitor {
public:
    ObjectP visit(const ast::Program &program);

    static ObjectP visit(const ast::IntegerLiteral &integer_literal);

    static ObjectP visit(const ast::BooleanExpression &boolean_expression);

    ObjectP visit(const ast::ExpressionStatement &expression_statement);

    ObjectP visit(const std::vector<const ast::Statement *> &statements);

    ObjectP visit(const ast::PrefixExpression &prefix_expression);

    static ObjectP visit(const ast::InfixExpression &infix_expression);

protected:
    static ObjectP evaluate_prefix_expression(const std::string &prefix_operator, ObjectP &&right_evaluated);

    static ObjectP evaluate_bang_operator(ObjectP &&right_evaluated);

    static ObjectP evaluate_minus_prefix_operator(ObjectP &&right_evaluated);

    static eval::ObjectP evaluate_infix_expression(const std::string &infix_operator, std::unique_ptr<objects::Object> left,
                                            std::unique_ptr<objects::Object> right);

    static eval::ObjectP evaluate_infix_expression(const std::string &infix_operator, const objects::Integer *left,
                                            const objects::Integer *right);
};

}

#endif //MATCHBOXDB_EVAL_H
