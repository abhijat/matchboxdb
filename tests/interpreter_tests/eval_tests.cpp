#include <gtest/gtest.h>

#include "../../src/interpreter/object.h"
#include "../../src/interpreter/lexer.h"
#include "../../src/interpreter/parser.h"
#include "../../src/interpreter/eval.h"

std::unique_ptr<objects::Object> evaluate(const std::string &input) {
    parser::Parser p{lexer::Lexer{input}};
    auto program = p.parse();
    return eval::evaluate(program);
}

void assert_int_literal(const objects::Object *object, int64_t value) {
    const auto integer = dynamic_cast<const objects::Integer *>(object);
    ASSERT_NE(integer, nullptr);
    ASSERT_EQ(integer->value(), value);
}

void assert_bool_literal(const objects::Object *object, bool value) {
    const auto boolean = dynamic_cast<const objects::Boolean *>(object);
    ASSERT_NE(boolean, nullptr);
    ASSERT_EQ(boolean->value(), value);
}

TEST(Evaluator, IntExpr) {
    auto object = evaluate("5");
    assert_int_literal(object.get(), 5);
}

TEST(Evaluator, BoolExpr) {
    auto object = evaluate("true");
    assert_bool_literal(object.get(), true);
}

TEST(Evaluator, BangOperator) {
    std::vector<std::pair<std::string, bool>> test_data{
        {"!true",   false},
        {"!false",  true},
        {"!5",      false},
        {"!!true",  true},
        {"!!false", false},
        {"!!5",     true},
    };

    for (const auto&[input, output]: test_data) {
        auto object = evaluate(input);
        assert_bool_literal(object.get(), output);
    }
}