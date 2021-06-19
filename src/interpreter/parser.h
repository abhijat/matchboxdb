#ifndef MATCHBOXDB_PARSER_H
#define MATCHBOXDB_PARSER_H

#include <unordered_map>
#include <functional>
#include "lexer.h"
#include "program.h"
#include "expression.h"
#include "block_statement.h"

namespace parser {

using PrefixParserFn = std::function<std::optional<std::unique_ptr<ast::Expression>>()>;
using InfixParserFn = std::function<std::optional<std::unique_ptr<ast::Expression>>(std::unique_ptr<ast::Expression>)>;

enum class Precedence {
    LOWEST,
    EQUALS,
    LESSGT,
    SUM,
    PRODUCT,
    PREFIX,
    FNCALL,
};

static const std::map<token::TokenKind, Precedence> k_precedences = {
    {token::TokenKind::EQ,       Precedence::EQUALS},
    {token::TokenKind::NE,       Precedence::EQUALS},
    {token::TokenKind::LT,       Precedence::LESSGT},
    {token::TokenKind::GT,       Precedence::LESSGT},
    {token::TokenKind::PLUS,     Precedence::SUM},
    {token::TokenKind::MINUS,    Precedence::SUM},
    {token::TokenKind::SLASH,    Precedence::PRODUCT},
    {token::TokenKind::ASTERISK, Precedence::PRODUCT},
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

    Precedence peek_precedence() const;

    Precedence current_precedence() const;

    std::optional<std::unique_ptr<ast::Statement>> parse_expression_statement();

    std::optional<std::unique_ptr<ast::Expression>> parse_expression(Precedence precedence);

    std::optional<std::unique_ptr<ast::Expression>> parse_prefix_expression();

    std::optional<std::unique_ptr<ast::Expression>> parse_infix_expression(std::unique_ptr<ast::Expression> left);

    std::optional<std::unique_ptr<ast::Expression>> parse_boolean_expression();

    std::optional<std::unique_ptr<ast::Expression>> parse_integer_literal();

    std::optional<std::unique_ptr<ast::Expression>> parse_grouped_expression();

    std::optional<std::unique_ptr<ast::Expression>> parse_if_expression();

    ast::BlockStatement parse_block_statement();

protected:
    lexer::Lexer _lexer;
    token::Token _current_token{};
    token::Token _peek_token{};
    std::vector<std::string> _errors{};
    std::unordered_map<token::TokenKind, PrefixParserFn> _prefix_parsers{};
    std::unordered_map<token::TokenKind, InfixParserFn> _infix_parsers{};
};

}


#endif //MATCHBOXDB_PARSER_H
