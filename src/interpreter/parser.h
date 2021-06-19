#ifndef MATCHBOXDB_PARSER_H
#define MATCHBOXDB_PARSER_H

#include <unordered_map>
#include <functional>
#include "lexer.h"
#include "program.h"
#include "expression.h"

namespace parser {

using PrefixParserFn = std::function<std::optional<std::unique_ptr<ast::Expression>>()>;
using InfixParserFn = std::function<std::optional<std::unique_ptr<ast::Expression>>(const ast::Expression *)>;

enum class Precedence {
    LOWEST,
    EQUALS,
    LESSGT,
    SUM,
    PRODUCT,
    PREFIX,
    FNCALL,
};

class Parser {
public:
    Parser(lexer::Lexer lexer);

    ast::Program parse();

    const std::vector<std::string> &errors() const;

    void register_prefix_fn(token::TokenKind token_kind, PrefixParserFn fn);

    void register_infix_fn(token::TokenKind token_kind, InfixParserFn fn);

protected:
    std::optional<std::unique_ptr<ast::Expression>> parse_identifier();

    std::optional<std::unique_ptr<ast::Statement>> parse_statement();

    std::optional<std::unique_ptr<ast::Statement>> parse_let_statement();

    std::optional<std::unique_ptr<ast::Statement>> parse_return_statement();

    void next_token();

    bool current_token_is(token::TokenKind token_kind) const;

    bool expect_peek_token(token::TokenKind token_kind);

    void peek_error(token::TokenKind expected);

    void no_prefix_parser_fn_error(token::TokenKind token_kind);

    std::optional<std::unique_ptr<ast::Statement>> parse_expression_statement();

    std::optional<std::unique_ptr<ast::Expression>> parse_expression(Precedence precedence);

    std::optional<std::unique_ptr<ast::Expression>> parse_prefix_expression();

protected:
    lexer::Lexer _lexer;
    token::Token _current_token{};
    token::Token _peek_token{};
    std::vector<std::string> _errors{};
    std::unordered_map<token::TokenKind, PrefixParserFn> _prefix_parsers{};
    std::unordered_map<token::TokenKind, InfixParserFn> _infix_parsers{};

    std::optional<std::unique_ptr<ast::Expression>> parse_integer_literal();
};

}


#endif //MATCHBOXDB_PARSER_H
