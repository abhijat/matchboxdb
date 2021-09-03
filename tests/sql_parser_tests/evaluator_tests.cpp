#include <gtest/gtest.h>

#include "../../src/sql_parser/statement.h"
#include "../../src/sql_parser/expression_statement.h"
#include "../../src/sql_parser/parser.h"
#include "../../src/sql_parser/expression_visitor.h"
#include "../test_utilities.h"

ast::EvaluatingExpressionVisitor make_contextless_visitor() {
    tuple::Tuple t{{}};
    metadata::Metadata m{{},
                         {}};
    return ast::EvaluatingExpressionVisitor{t, m};
}

ast::EvaluatingExpressionVisitor make_visitor_with_context() {
    metadata::Metadata m{
        {"name",                 "age"},
        {metadata::Kind::String, metadata::Kind::UnsignedInt},
    };

    tuple::Tuple t{{std::string{"alucard"}, uint32_t{1000}}};

    return ast::EvaluatingExpressionVisitor{t, m};
}

template<typename T>
void assert_subtype(const T *t) {
    ASSERT_NE(t, nullptr);
}

ast::Expression *extract_expression(const ast::Statement *statement) {
    auto es = dynamic_cast<const ast::ExpressionStatement *>(statement);
    assert_subtype(es);
    return es->expression().get();
}

TEST(SimpleExpressionEval, Integer) {
    auto stmt = testutils::parse("5");
    auto expression = extract_expression(stmt.get());

    auto result = make_contextless_visitor().evaluate(*expression);
    ASSERT_EQ(std::get<uint32_t>(*result), 5);
}

TEST(SimpleExpressionEval, Boolean) {
    auto stmt = testutils::parse("false");
    auto expression = extract_expression(stmt.get());

    auto result = make_contextless_visitor().evaluate(*expression);
    ASSERT_EQ(std::get<bool>(*result), false);
}

TEST(SimpleExpressionEval, String) {
    auto stmt = testutils::parse("\"foobar\"");
    auto expression = extract_expression(stmt.get());

    auto result = make_contextless_visitor().evaluate(*expression);
    ASSERT_EQ(std::get<std::string>(*result), "foobar");
}

TEST(SimpleExpressionEval, Identifier) {
    auto stmt = testutils::parse("name");
    auto expression = extract_expression(stmt.get());

    auto result = make_visitor_with_context().evaluate(*expression);
    ASSERT_EQ(std::get<std::string>(*result), "alucard");
}

TEST(BooleanExpressionWithContext, ChainedExpressions) {
    auto visitor = make_visitor_with_context();

    for (const auto&[input, expected]: std::vector<std::pair<std::string, bool>>{
        {R"(name = "alucard")",                  true},
        {R"(name = "alucard" AND age = 1000)",   true},
        {R"(name != "alucard")",                 false},
        {R"(name = "alucard" AND age != 1000)", false},
        {R"(name = "alucard" AND age > 1)",     true},
        {R"(name != "alucard" AND age = 1000)", false},
        {R"(name != "alucard" OR age = 1000)",  true},
        {R"(name != "alucard" OR age != 1000)",  false},
    }) {
        auto statement = testutils::parse(input);
        auto expr = extract_expression(statement.get());
        ASSERT_EQ(expected, std::get<bool>(*visitor.evaluate(*expr)));
    }
}
