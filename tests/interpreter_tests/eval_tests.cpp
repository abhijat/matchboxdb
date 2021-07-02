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