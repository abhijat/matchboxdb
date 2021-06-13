#include <gtest/gtest.h>
#include "../../src/interpreter/parser.h"
#include "../../src/interpreter/let_statement.h"
#include "../../src/interpreter/return_statement.h"

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