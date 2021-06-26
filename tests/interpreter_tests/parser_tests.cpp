#include <gtest/gtest.h>
#include "../../src/interpreter/parser.h"
#include "../../src/interpreter/let_statement.h"
#include "../../src/interpreter/return_statement.h"
#include "../../src/interpreter/expression_statement.h"
#include "../../src/interpreter/integer_literal.h"
#include "../../src/interpreter/prefix_expression.h"
#include "../../src/interpreter/infix_expression.h"
#include "../../src/interpreter/boolean_expression.h"
#include "../../src/interpreter/if_expression.h"
#include "../../src/interpreter/function_expression.h"
#include "../../src/interpreter/call_expression.h"

void assert_let_statement(const ast::Statement *statement, std::string_view identifier_name) {
    ASSERT_EQ(statement->token_literal(), "let");
    const auto *let_statement = dynamic_cast<const ast::LetStatement *>(statement);
    ASSERT_NE(let_statement, nullptr);
    ASSERT_EQ(let_statement->name().token_literal(), identifier_name);
    ASSERT_EQ(let_statement->name().value(), identifier_name);
}

void check_parser_errors(const parser::Parser &p) {
    auto errors = p.errors();
    if (errors.empty()) {
        return;
    }

    std::cout << "found " << errors.size() << " parser errors\n";
    for (const auto &error : errors) {
        std::cout << "parser error: " << error << "\n";
    }

    FAIL() << "parser errors found\n";
}

void assert_identifier(const ast::Identifier *identifier, const std::string &value) {
    ASSERT_EQ(identifier->value(), value);
    ASSERT_EQ(identifier->token_literal(), value);
}

void assert_identifier(const ast::Expression *expression, const std::string &value) {
    const auto *identifier = dynamic_cast<const ast::Identifier *>(expression);
    ASSERT_NE(identifier, nullptr);

    assert_identifier(identifier, value);
}


void assert_integer_literal(const ast::Expression *expression, int64_t value) {
    const auto *integer_literal = dynamic_cast<const ast::IntegerLiteral *>(expression);
    ASSERT_NE(integer_literal, nullptr);
    ASSERT_EQ(integer_literal->value(), value);
}

void assert_boolean_expression(const ast::Expression *expression, bool value) {
    const auto *boolean_expression = dynamic_cast<const ast::BooleanExpression *>(expression);
    ASSERT_NE(boolean_expression, nullptr);
    ASSERT_EQ(boolean_expression->value(), value);
}

void assert_literal_expression(const ast::Expression *expression, int64_t expected) {
    assert_integer_literal(expression, expected);
}

void assert_literal_expression(const ast::Expression *expression, std::string expected) {
    assert_identifier(expression, expected);
}

void assert_literal_expression(const ast::Expression *expression, bool value) {
    assert_boolean_expression(expression, value);
}

template<typename ExpressionType>
void assert_infix_expression(const ast::Expression *expression, ExpressionType left, ExpressionType right,
                             const std::string &infix_operator) {
    const auto *infix_expression = dynamic_cast<const ast::InfixExpression *>(expression);
    ASSERT_NE(infix_expression, nullptr);

    assert_literal_expression(infix_expression->left(), left);
    assert_literal_expression(infix_expression->right(), right);
    ASSERT_EQ(infix_expression->infix_operator(), infix_operator);
}

const ast::Expression *assert_expression_statement_and_extract_expression(const ast::Statement *statement) {
    const auto *expression_statement = dynamic_cast<const ast::ExpressionStatement *>(statement);
    if (expression_statement == nullptr) {
        throw std::invalid_argument{"Failed because statement is not an expression statement"};
    }

    return expression_statement->expression();
}

TEST(Parser, LetStatementParsing) {
    std::string input{R"S(

    let x = 5;
    let y = 10;

    let foobar = 838383;

)S"};

    parser::Parser p{lexer::Lexer{input}};
    auto program = p.parse();
    check_parser_errors(p);

    ASSERT_EQ(program.statements().size(), 3);

    std::vector<std::string> expected{"x", "y", "foobar"};
    for (auto i = 0; i < expected.size(); ++i) {
        assert_let_statement(program.statements().at(i).get(), expected.at(i));
    }
}

TEST(Parser, ReturnStatements) {
    std::string input{R"S(

    return 5;

    return 10;

    return 993322;

)S"};

    parser::Parser p{lexer::Lexer{input}};
    auto program = p.parse();
    check_parser_errors(p);

    ASSERT_EQ(program.statements().size(), 3);
    for (const auto &statement : program.statements()) {
        const auto *rs = dynamic_cast<const ast::ReturnStatement *>(statement.get());
        EXPECT_NE(rs, nullptr);
        EXPECT_EQ(rs->token_literal(), "return");
    }
}

TEST(Parser, Stringify) {
    ast::LetStatement ls{
        {token::TokenKind::LET, "let"},
        ast::Identifier{{token::TokenKind::IDENT, "lhs"}, "lhs"},
        std::make_unique<ast::Identifier>(ast::Identifier{{token::TokenKind::IDENT, "rhs"}, "rhs"})
    };

    std::stringstream ss;
    ss << ls;

    ASSERT_EQ(ss.str(), "let lhs = rhs;");
}

TEST(Parser, IndentifierExpression) {
    parser::Parser p{lexer::Lexer{"foobar;"}};
    auto program = p.parse();
    check_parser_errors(p);

    ASSERT_EQ(program.statements().size(), 1);

    const auto *expression_statement = dynamic_cast<const ast::ExpressionStatement *>(program.statements().at(0).get());
    ASSERT_NE(expression_statement, nullptr);

    assert_identifier(expression_statement->expression(), "foobar");
}

TEST(Parser, IntegerLiteralExpression) {
    parser::Parser p{lexer::Lexer{"100;"}};
    auto program = p.parse();
    check_parser_errors(p);

    ASSERT_EQ(program.statements().size(), 1);

    const auto *expression_statement = dynamic_cast<const ast::ExpressionStatement *>(program.statements().at(0).get());
    ASSERT_NE(expression_statement, nullptr);

    assert_integer_literal(expression_statement->expression(), 100);
}

TEST(Parser, PrefixExpression) {
    {
        parser::Parser p{lexer::Lexer{"!5;"}};
        auto program = p.parse();
        check_parser_errors(p);

        ASSERT_EQ(program.statements().size(), 1);
        const auto *expression_statement = dynamic_cast<const ast::ExpressionStatement *>(program.statements().at(
            0).get());
        ASSERT_NE(expression_statement, nullptr);

        const auto *prefix_expression = dynamic_cast<const ast::PrefixExpression *>(expression_statement->expression());
        ASSERT_NE(prefix_expression, nullptr);

        ASSERT_EQ(prefix_expression->prefix_operator(), "!");
        assert_integer_literal(prefix_expression->right(), 5);
    }

    {
        parser::Parser p{lexer::Lexer{"-1115;"}};
        auto program = p.parse();
        check_parser_errors(p);

        ASSERT_EQ(program.statements().size(), 1);
        const auto *expression_statement = dynamic_cast<const ast::ExpressionStatement *>(program.statements().at(
            0).get());
        ASSERT_NE(expression_statement, nullptr);

        const auto *prefix_expression = dynamic_cast<const ast::PrefixExpression *>(expression_statement->expression());
        ASSERT_NE(prefix_expression, nullptr);

        ASSERT_EQ(prefix_expression->prefix_operator(), "-");
        assert_integer_literal(prefix_expression->right(), 1115);
    }
}


TEST(Parser, InfixExpression) {
    struct TestValue {
        std::string input;
        int64_t left;
        int64_t right;
        std::string infix_operator;
    };

    std::vector<TestValue> tests{
        {"5 + 5;",  5, 5, "+"},
        {"5 - 5;",  5, 5, "-"},
        {"5 * 5;",  5, 5, "*"},
        {"5 / 5;",  5, 5, "/"},
        {"5 < 5;",  5, 5, "<"},
        {"5 > 5;",  5, 5, ">"},
        {"5 == 5;", 5, 5, "=="},
        {"5 != 5;", 5, 5, "!="},
    };

    for (const auto &t: tests) {
        parser::Parser p{lexer::Lexer{t.input}};
        auto program = p.parse();
        check_parser_errors(p);

        ASSERT_EQ(program.statements().size(), 1) << "failed for " << t.input;

        const auto *expression = dynamic_cast<const ast::ExpressionStatement *>(program.statements().at(0).get());
        ASSERT_NE(expression, nullptr) << "failed for " << t.input;
        assert_infix_expression(expression->expression(), t.left, t.right, t.infix_operator);
    }

    parser::Parser p{lexer::Lexer{"alice * bob"}};
    auto program = p.parse();
    check_parser_errors(p);

    ASSERT_EQ(program.statements().size(), 1);
    const auto *expression = dynamic_cast<const ast::ExpressionStatement *>(program.statements().at(0).get());
    ASSERT_NE(expression, nullptr);
    assert_infix_expression(expression->expression(), std::string{"alice"}, std::string{"bob"}, "*");
}

TEST(Parser, PrecedenceParsing) {
    std::vector<std::pair<std::string, std::string>> tests{
        {"-a * b",                     "((-a) * b)"},
        {"!-a",                        "(!(-a))"},
        {"a + b + c",                  "((a + b) + c)"},
        {"a + b - c",                  "((a + b) - c)"},
        {"a * b * c",                  "((a * b) * c)"},
        {"a * b / c",                  "((a * b) / c)"},
        {"a + b / c",                  "(a + (b / c))"},
        {"a + b * c + d / e - f",      "(((a + (b * c)) + (d / e)) - f)"},
        {"3 + 4; -5 * 5",              "(3 + 4)((-5) * 5)"},
        {"5 > 4 == 3 < 4",             "((5 > 4) == (3 < 4))"},
        {"5 < 4 != 3 > 4",             "((5 < 4) != (3 > 4))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
        {"true",                       "true"},
        {"false",                      "false"},
        {"3 > 5 == false",             "((3 > 5) == false)"},
        {"4 < 100 == true",            "((4 < 100) == true)"},
        {"1 + (2 + 3) + 4",            "((1 + (2 + 3)) + 4)"},
        {"(5 + 5) * 2",                "((5 + 5) * 2)"},
        {"2 / (5 + 5)",                "(2 / (5 + 5))"},
        {"-(5 + 5)",                   "(-(5 + 5))"},
        {"!(true == true)",            "(!(true == true))"},
    };

    for (const auto&[input, output]: tests) {
        parser::Parser p{lexer::Lexer{input}};
        auto program = p.parse();
        check_parser_errors(p);

        std::stringstream ss;
        ss << program;
        ASSERT_EQ(ss.str(), output);
    }
}

TEST(Parser, BooleanExpression) {
    std::vector<std::pair<std::string, bool>> tests{
        {"true;",  true},
        {"false;", false}
    };

    for (const auto&[input, output]: tests) {
        parser::Parser p{lexer::Lexer{input}};
        auto program = p.parse();
        check_parser_errors(p);

        ASSERT_EQ(program.statements().size(), 1);
        auto expression = assert_expression_statement_and_extract_expression(program.statements().at(0).get());
        assert_boolean_expression(expression, output);
    }
}

TEST(Parser, IfExpression) {
    parser::Parser p{lexer::Lexer{"if (x < y) { x } else {y}"}};
    auto program = p.parse();
    check_parser_errors(p);

    ASSERT_EQ(program.statements().size(), 1);
    auto expression = assert_expression_statement_and_extract_expression(program.statements().at(0).get());
    const auto *if_expression = dynamic_cast<const ast::IfExpression *>(expression);
    ASSERT_NE(if_expression, nullptr);

    assert_infix_expression(if_expression->condition(), std::string{"x"}, std::string{"y"}, "<");

    const auto &consequence = if_expression->consequence();
    ASSERT_EQ(consequence.statements().size(), 1);

    const auto *cexpr = assert_expression_statement_and_extract_expression(consequence.statements().at(0).get());
    assert_literal_expression(cexpr, std::string{"x"});

    const auto &alternative = if_expression->alternative();
    ASSERT_EQ(alternative->statements().size(), 1);

    const auto *alt = assert_expression_statement_and_extract_expression(alternative->statements().at(0).get());
    assert_literal_expression(alt, std::string{"y"});
}

TEST(Parser, FnExpression) {
    parser::Parser p{lexer::Lexer{"fn(x, y) { x + y; }"}};
    auto program = p.parse();
    check_parser_errors(p);

    ASSERT_EQ(program.statements().size(), 1);
    auto expression = assert_expression_statement_and_extract_expression(program.statements().at(0).get());
    const auto *fn_expr = dynamic_cast<const ast::FunctionExpression *>(expression);
    ASSERT_NE(fn_expr, nullptr);

    ASSERT_EQ(fn_expr->parameters().size(), 2);
    assert_identifier(&fn_expr->parameters().at(0), "x");
    assert_identifier(&fn_expr->parameters().at(1), "y");

    ASSERT_EQ(fn_expr->body().statements().size(), 1);

    const auto *body = assert_expression_statement_and_extract_expression(fn_expr->body().statements().at(0).get());
    assert_infix_expression(body, std::string{"x"}, std::string{"y"}, "+");
}

TEST(Parser, FnParams) {
    struct test {
        std::string input;
        std::vector<std::string> expected;
    };

    std::vector<test> tests{
        {"fn () {};",        {}},
        {"fn (x) {};",       {"x"}},
        {"fn (x, y, z) {};", {"x", "y", "z"}},
    };

    for (const auto &t: tests) {
        parser::Parser p{lexer::Lexer{t.input}};
        auto program = p.parse();
        check_parser_errors(p);

        auto expression = assert_expression_statement_and_extract_expression(program.statements().at(0).get());
        const auto *fn_expr = dynamic_cast<const ast::FunctionExpression *>(expression);
        ASSERT_NE(fn_expr, nullptr);

        ASSERT_EQ(fn_expr->parameters().size(), t.expected.size());
        for (auto i = 0; i < t.expected.size(); ++i) {
            ASSERT_EQ(t.expected[i], fn_expr->parameters()[i].token_literal());
        }
    }
}

TEST(Parser, CallExpr) {
    parser::Parser p{lexer::Lexer{"foobar(1, 2 * 3, 4 + 5);"}};
    auto program = p.parse();
    check_parser_errors(p);

    ASSERT_EQ(program.statements().size(), 1);

    const auto *expr = assert_expression_statement_and_extract_expression(program.statements().at(0).get());
    const auto *call_expr = dynamic_cast<const ast::CallExpression *>(expr);
    ASSERT_NE(call_expr, nullptr);

    const auto *callable = dynamic_cast<const ast::Identifier *>(call_expr->callable().get());
    ASSERT_NE(callable, nullptr);

    ASSERT_EQ(callable->token_literal(), "foobar");

    assert_literal_expression(call_expr->arguments().at(0).get(), int64_t{1});
    assert_infix_expression<int64_t>(call_expr->arguments().at(1).get(), 2, 3, "*");
    assert_infix_expression<int64_t>(call_expr->arguments().at(2).get(), 4, 5, "+");
}