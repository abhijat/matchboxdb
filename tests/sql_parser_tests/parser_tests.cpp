#include <gtest/gtest.h>

#include "../../src/sql_parser/lexer.h"
#include "../../src/sql_parser/parser.h"
#include "../../src/sql_parser/statement.h"
#include "../../src/sql_parser/expression_statement.h"
#include "../../src/sql_parser/integer_literal.h"
#include "../../src/sql_parser/Identifier.h"
#include "../../src/sql_parser/infix_expression.h"
#include "../../src/sql_parser/boolean_literal.h"

std::unique_ptr<ast::Statement> parse(const std::string &s) {
    return parser::Parser{lexer::Lexer{s}}.parse();
}

void assert_int_literal(const ast::Expression *expression, int64_t value) {
    auto integer_literal = dynamic_cast<const ast::IntegerLiteral *>(expression);
    ASSERT_NE(integer_literal, nullptr);

    ASSERT_EQ(integer_literal->value(), value);
}

void assert_int_literal(const ast::Statement *statement, int64_t value) {
    auto expression_statement = dynamic_cast<const ast::ExpressionStatement *>(statement);
    ASSERT_NE(expression_statement, nullptr);

    const auto expression = expression_statement->expression().get();
    assert_int_literal(expression, value);
}

void assert_infix_expression(const ast::Statement *statement, const std::string &op, int64_t lhs, int64_t rhs) {
    auto expression_statement = dynamic_cast<const ast::ExpressionStatement *>(statement);
    ASSERT_NE(expression_statement, nullptr);

    auto infix_expression = dynamic_cast<const ast::InfixExpression *>(expression_statement->expression().get());
    ASSERT_NE(infix_expression, nullptr);

    assert_int_literal(infix_expression->left(), lhs);
    assert_int_literal(infix_expression->right(), rhs);
    ASSERT_EQ(infix_expression->infix_operator(), op);
}

TEST(Parser, IntLiteral) {
    auto statement = parse("123");
    assert_int_literal(statement.get(), 123);
}

TEST(Parser, Identifiers) {
    auto statement = parse("foobar");
    const auto s = dynamic_cast<ast::ExpressionStatement *>(statement.get());
    ASSERT_NE(s, nullptr);

    const auto expr = dynamic_cast<ast::Identifier *>(s->expression().get());
    ASSERT_NE(expr, nullptr);

    ASSERT_EQ(expr->value(), "foobar");
}

TEST(Parser, InfixExpressionsSimple) {
    struct TestData {
        std::string input;
        std::string op;
        int64_t lhs;
        int64_t rhs;
    };

    std::vector<TestData> test_data{
        {"5 AND 6", "AND", 5, 6},
        {"5 OR 6",  "OR",  5, 6},
        {"5 = 6",   "=",   5, 6},
        {"5 != 6",  "!=",  5, 6},
        {"5 > 6",   ">",   5, 6},
        {"5 < 6",   "<",   5, 6},
    };

    for (const auto &td: test_data) {
        auto statement = parse(td.input);
        assert_infix_expression(statement.get(), td.op, td.lhs, td.rhs);
    }
}

TEST(Parser, Grouping) {
    for (const auto &[input, output]: std::vector<std::pair<std::string, std::string>>{
        {"a AND b OR c",  "((a AND b) OR c)"},
        {"a OR b AND c",  "(a OR (b AND c))"},
        {"a AND b AND c", "((a AND b) AND c)"},
        {"a OR b OR c",   "((a OR b) OR c)"},
    }) {
        auto statement = parse(input);
        auto expression_statement = dynamic_cast<const ast::ExpressionStatement *>(statement.get());
        ASSERT_NE(expression_statement, nullptr);

        auto infix_expression = dynamic_cast<const ast::InfixExpression *>(expression_statement->expression().get());
        ASSERT_NE(infix_expression, nullptr);

        std::stringstream ss;
        ss << *infix_expression;

        ASSERT_EQ(ss.str(), output) << input;
    }
}

void assert_boolean_literal(const ast::Statement *statement, bool value) {
    auto expression_statement = dynamic_cast<const ast::ExpressionStatement *>(statement);
    ASSERT_NE(expression_statement, nullptr);

    auto bool_expression = dynamic_cast<const ast::BooleanLiteral *>(expression_statement->expression().get());
    ASSERT_NE(bool_expression, nullptr);

    ASSERT_EQ(bool_expression->value(), value);
}

TEST(Parser, Booleans) {
    for (const auto &[input, output]: std::vector<std::pair<std::string, bool>>{
        {"true",  true},
        {"false", false},
    }) {
        assert_boolean_literal(parse(input).get(), output);
    }
}
