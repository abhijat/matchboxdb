#include <gtest/gtest.h>
#include "../../src/sql_parser/boolean_literal.h"
#include "../../src/sql_parser/create_statement.h"
#include "../../src/sql_parser/expression_statement.h"
#include "../../src/sql_parser/infix_expression.h"
#include "../../src/sql_parser/integer_literal.h"
#include "../../src/sql_parser/parser.h"
#include "../../src/sql_parser/select_statement.h"
#include "../../src/sql_parser/update_statement.h"
#include "../../src/sql_parser/insert_statement.h"
#include "../../src/sql_parser/delete_statement.h"
#include "../../src/sql_parser/drop_statement.h"
#include "../../src/sql_parser/describe_statement.h"
#include "../test_utilities.h"

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

void assert_infix_expression(const ast::Statement *statement,
                             const std::string &op, int64_t lhs, int64_t rhs) {
    auto expression_statement =
        dynamic_cast<const ast::ExpressionStatement *>(statement);
    ASSERT_NE(expression_statement, nullptr);

    auto infix_expression = dynamic_cast<const ast::InfixExpression *>(
        expression_statement->expression().get());
    ASSERT_NE(infix_expression, nullptr);

    assert_int_literal(infix_expression->left(), lhs);
    assert_int_literal(infix_expression->right(), rhs);
    ASSERT_EQ(infix_expression->infix_operator(), op);
}

TEST(Parser, IntLiteral) {
    auto statement = testutils::parse("123");
    assert_int_literal(statement.get(), 123);
}

TEST(Parser, Identifiers) {
    auto statement = testutils::parse("foobar");
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
        auto statement = testutils::parse(td.input);
        assert_infix_expression(statement.get(), td.op, td.lhs, td.rhs);
    }
}

void assert_infix_is_string(const ast::Statement *statement,
                            const std::string &repr) {
    auto expression_statement =
        dynamic_cast<const ast::ExpressionStatement *>(statement);
    ASSERT_NE(expression_statement, nullptr);

    auto infix_expression = dynamic_cast<const ast::InfixExpression *>(
        expression_statement->expression().get());
    ASSERT_NE(infix_expression, nullptr);

    std::stringstream ss;
    ss << *infix_expression;

    ASSERT_EQ(ss.str(), repr) << repr;
}

TEST(Parser, Grouping) {
    for (const auto &[input, output]:
        std::vector<std::pair<std::string, std::string>>{
            {"a AND b OR c",  "((a AND b) OR c)"},
            {"a OR b AND c",  "(a OR (b AND c))"},
            {"a AND b AND c", "((a AND b) AND c)"},
            {"a OR b OR c",   "((a OR b) OR c)"},
        }) {
        auto statement = testutils::parse(input);
        assert_infix_is_string(statement.get(), output);
    }
}

void assert_boolean_literal(const ast::Statement *statement, bool value) {
    auto expression_statement =
        dynamic_cast<const ast::ExpressionStatement *>(statement);
    ASSERT_NE(expression_statement, nullptr);

    auto bool_expression = dynamic_cast<const ast::BooleanLiteral *>(
        expression_statement->expression().get());
    ASSERT_NE(bool_expression, nullptr);

    ASSERT_EQ(bool_expression->value(), value);
}

TEST(Parser, Booleans) {
    for (const auto &[input, output]: std::vector<std::pair<std::string, bool>>{
        {"true",  true},
        {"false", false},
    }) {
        assert_boolean_literal(testutils::parse(input).get(), output);
    }
}

TEST(Parser, GroupedExpressions) {
    for (const auto &[input, output]:
        std::vector<std::pair<std::string, std::string>>{
            {"a AND (b OR c)", "(a AND (b OR c))"},
        }) {
        auto statement = testutils::parse(input);
        assert_infix_is_string(statement.get(), output);
    }
}

TEST(Parser, SelectStatement) {
    auto statement = testutils::parse("SELECT name, author FROM western_canon WHERE foo = 1 "
                                      "AND bar = 22 OR x = FALSE AND name != 47;");
    const auto select_statement =
        dynamic_cast<const ast::SelectStatement *>(statement.get());
    ASSERT_NE(select_statement, nullptr);
    std::stringstream ss;
    ss << *select_statement;
    ASSERT_EQ(ss.str(),
              "SELECT {name, author} FROM [western_canon] WHERE (((foo = 1) AND "
              "(bar = 22)) OR ((x = FALSE) AND (name != 47)))");
}

TEST(Parser, CreateStatement) {
    auto statement = testutils::parse("CREATE TABLE person (name STRING, age UNSIGNED_INT, "
                                      "is_replicant BOOLEAN);");
    const auto create_statement =
        dynamic_cast<const ast::CreateStatement *>(statement.get());
    ASSERT_NE(create_statement, nullptr);
    std::stringstream ss;
    ss << *create_statement;
    ASSERT_EQ(ss.str(), "CREATE TABLE person( {name: String} {age: UnsignedInt} "
                        "{is_replicant: Boolean} )");
}

TEST(Parser, UpdateStatement) {
    auto statement =
        testutils::parse("UPDATE person SET name = 123, age = 11111, is_replicant = FALSE;");
    const auto update_statement =
        dynamic_cast<const ast::UpdateStatement *>(statement.get());
    ASSERT_NE(update_statement, nullptr);
    std::stringstream ss;
    ss << *update_statement;
    ASSERT_EQ(
        ss.str(),
        "UPDATE [person] SET {name = 123, age = 11111, is_replicant = FALSE}");
}

TEST(Parser, InsertStatement) {
    auto statement = testutils::parse(R"(INSERT INTO person VALUES ("cujo", 7, FALSE))");
    const auto insert_statement = dynamic_cast<const ast::InsertStatement *>(statement.get());
    ASSERT_NE(insert_statement, nullptr);

    std::stringstream ss;
    ss << *insert_statement;
    ASSERT_EQ(ss.str(), R"(INSERT INTO [person] VALUES {"cujo", 7, FALSE})");
}

TEST(Parser, DeleteStatement) {
    auto statement = testutils::parse(R"(DELETE FROM person WHERE name = "cujo" AND age = 7)");
    const auto delete_statement = dynamic_cast<const ast::DeleteStatement *>(statement.get());
    ASSERT_NE(delete_statement, nullptr);

    std::stringstream ss;
    ss << *delete_statement;
    ASSERT_EQ(ss.str(), R"(DELETE FROM [person] WHERE ((name = "cujo") AND (age = 7)))");
}

TEST(Parser, DropStatement) {
    auto statement = testutils::parse(R"(DROP TABLE person)");
    const auto delete_statement = dynamic_cast<const ast::DropStatement *>(statement.get());
    ASSERT_NE(delete_statement, nullptr);

    ASSERT_EQ(delete_statement->table_name(), "person");
}

TEST(Parser, DescribeStatement) {
    auto statement = testutils::parse(R"(DESCRIBE TABLE person)");
    const auto describe_statement = dynamic_cast<const ast::DescribeStatement *>(statement.get());
    ASSERT_NE(describe_statement, nullptr);
    ASSERT_EQ(describe_statement->table()->table_name(), "person");
}
