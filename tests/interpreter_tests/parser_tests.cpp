#include <gtest/gtest.h>
#include "../../src/interpreter/parser.h"
#include "../../src/interpreter/let_statement.h"

void assert_let_statement(const ast::Statement *statement, std::string_view identifier_name) {
    ASSERT_EQ(statement->token_literal(), "let");
    const auto *let_statement = dynamic_cast<const ast::LetStatement *>(statement);
    ASSERT_EQ(let_statement->name().token_literal(), identifier_name);
    ASSERT_EQ(let_statement->name().value(), identifier_name);
}

TEST(Parser, LetStatementParsing) {
    std::string input{R"S(

    let x = 5;
    let y = 10;

    let foobar = 838383;

)S"};

    parser::Parser p{lexer::Lexer{input}};
    auto program = p.parse();

    ASSERT_EQ(program.statements().size(), 3);

    std::vector<std::string> expected{"x", "y", "foobar"};
    for (auto i = 0; i < expected.size(); ++i) {
        assert_let_statement(program.statements().at(i).get(), expected.at(i));
    }
}
