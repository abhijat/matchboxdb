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

TEST(Evaluator, IntExpr) {
    auto object = evaluate("5");
    ASSERT_NE(object, nullptr);
    const auto integer = dynamic_cast<const objects::Integer *>(object.get());
    ASSERT_NE(integer, nullptr);
    ASSERT_EQ(integer->value(), 5);
}

TEST(Evaluator, BoolExpr) {
    auto object = evaluate("true");
    ASSERT_NE(object, nullptr);
    const auto boolean = dynamic_cast<const objects::Boolean *>(object.get());
    ASSERT_NE(boolean, nullptr);
    ASSERT_EQ(boolean->value(), true);
}
