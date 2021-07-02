#ifndef MATCHBOXDB_EVAL_H
#define MATCHBOXDB_EVAL_H

#include <memory>
#include <vector>

#include "object.h"
#include "node.h"
#include "expression_statement.h"

namespace ast {
class Program;

class BooleanExpression;

class IntegerLiteral;
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
