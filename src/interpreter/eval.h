#ifndef MATCHBOXDB_EVAL_H
#define MATCHBOXDB_EVAL_H

#include <memory>
#include <vector>

namespace objects {
class Object;
}

namespace ast {
class Program;

class BooleanExpression;

class IntegerLiteral;

class ExpressionStatement;

class Statement;

class Node;
}

namespace eval {

std::unique_ptr<objects::Object> evaluate(const ast::Node &node);

class Visitor {
public:
    std::unique_ptr<objects::Object> visit(const ast::Program &program);

    static std::unique_ptr<objects::Object> visit(const ast::IntegerLiteral &integer_literal);

    static std::unique_ptr<objects::Object> visit(const ast::BooleanExpression &boolean_expression);

    std::unique_ptr<objects::Object> visit(const ast::ExpressionStatement &expression_statement);

    std::unique_ptr<objects::Object> visit(const std::vector<const ast::Statement *> &statements);
};

}

#endif //MATCHBOXDB_EVAL_H
