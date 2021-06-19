#include <gtest/gtest.h>
#include "../../src/interpreter/parser.h"
#include "../../src/interpreter/let_statement.h"
#include "../../src/interpreter/return_statement.h"
#include "../../src/interpreter/expression_statement.h"
#include "../../src/interpreter/integer_literal.h"
#include "../../src/interpreter/prefix_expression.h"
#include "../../src/interpreter/infix_expression.h"

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

    const auto *identifier = dynamic_cast<const ast::Identifier *>(expression_statement->expression());
    ASSERT_NE(identifier, nullptr);

    ASSERT_EQ(identifier->token_literal(), "foobar");
}

void assert_is_integer_literal(const ast::Expression *expression, int64_t value) {
    const auto *identifier = dynamic_cast<const ast::IntegerLiteral *>(expression);
    ASSERT_NE(identifier, nullptr);
    ASSERT_EQ(identifier->value(), value);
}

TEST(Parser, IntegerLiteralExpression) {
    parser::Parser p{lexer::Lexer{"100;"}};
    auto program = p.parse();
    check_parser_errors(p);

    ASSERT_EQ(program.statements().size(), 1);

    const auto *expression_statement = dynamic_cast<const ast::ExpressionStatement *>(program.statements().at(0).get());
    ASSERT_NE(expression_statement, nullptr);

    assert_is_integer_literal(expression_statement->expression(), 100);
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
        assert_is_integer_literal(prefix_expression->right(), 5);
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
        assert_is_integer_literal(prefix_expression->right(), 1115);
    }
}

void assert_infix_expression(const ast::InfixExpression *expression, int64_t left, int64_t right,
                             const std::string &infix_operator) {
    assert_is_integer_literal(expression->left(), left);
    assert_is_integer_literal(expression->right(), right);
    ASSERT_EQ(expression->infix_operator(), infix_operator);
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

        const auto *infix = dynamic_cast<const ast::InfixExpression *>(expression->expression());
        ASSERT_NE(infix, nullptr) << "failed for " << t.input;

        assert_infix_expression(infix, t.left, t.right, t.infix_operator);
    }
}