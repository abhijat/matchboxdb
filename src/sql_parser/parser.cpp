#include "parser.h"

#include "statement.h"
#include "select_statement.h"
#include "integer_literal.h"
#include "expression_statement.h"
#include "Identifier.h"
#include "infix_expression.h"

#include <utility>

parser::Parser::Parser(lexer::Lexer lexer)
    : _lexer(std::move(lexer)),
      _current_token(token::Kind::EndOfInput, ""),
      _peek_token(token::Kind::EndOfInput, "") {

    register_prefix(token::Kind::Integer, [&]() {
        return parse_integer_literal();
    });

    register_prefix(token::Kind::Identifier, [&]() {
        return parse_identifier();
    });

    for (const auto kind: {
        token::Kind::NE,
        token::Kind::Equals,
        token::Kind::GT,
        token::Kind::LT,
        token::Kind::And,
        token::Kind::Or,
    }) {
        register_infix(kind, [&](ExpressionP expression_p) {
            return parse_infix_expression(std::move(expression_p));
        });
    }

    next_token();
    next_token();
}

void parser::Parser::next_token() {
    _current_token = _peek_token;
    _peek_token = _lexer.next_token();
}

std::unique_ptr<ast::Statement> parser::Parser::parse() {
    return parse_statement();
}

std::unique_ptr<ast::Statement> parser::Parser::parse_statement() {
    switch (_current_token.kind()) {
        case token::Kind::Select:
            return parse_select_statement();
        default:
            return parse_expression_statement();
    }
}

std::unique_ptr<ast::SelectStatement> parser::Parser::parse_select_statement() {
    auto expressions = parse_expression_list();
    std::optional<ast::Table> table{};
    std::optional<std::unique_ptr<ast::Expression>> where_clause{};
    if (peek_token_is(token::Kind::From)) {
        table = parse_table_name();
        if (peek_token_is(token::Kind::Where)) {
            where_clause = parse_expression(Precedence::Lowest);
        }
    }

    return std::make_unique<ast::SelectStatement>(std::move(expressions), table, std::move(where_clause));
}

std::vector<std::unique_ptr<ast::Expression>> parser::Parser::parse_expression_list() {
    std::vector<std::unique_ptr<ast::Expression>> expressions{};

    auto expression = parse_expression(Precedence::Lowest);
    if (!expression) {
        return expressions;
    }

    expressions.push_back(std::move(*expression));

    while (current_token_is(token::Kind::Comma)) {
        expression = parse_expression(Precedence::Lowest);
        if (!expression) {
            return expressions;
        }

        expressions.push_back(std::move(*expression));
    }

    return expressions;
}

bool parser::Parser::peek_token_is(token::Kind kind) const {
    return _peek_token.kind() == kind;
}

std::optional<ast::Table> parser::Parser::parse_table_name() {
    next_token();
    return ast::Table{_current_token.literal()};
}

std::optional<std::unique_ptr<ast::Expression>> parser::Parser::parse_expression(Precedence precedence) {
    auto pos = _prefix_parsers.find(_current_token.kind());
    if (pos == _prefix_parsers.end()) {
        return {};
    }

    auto lhs = pos->second();

    while (!peek_token_is(token::Kind::Semicolon) && precedence < peek_precedence()) {
        auto infix_pos = _infix_parsers.find(_peek_token.kind());
        if (infix_pos == _infix_parsers.end()) {
            return lhs;
        }

        next_token();
        lhs = infix_pos->second(std::move(lhs));
    }

    return lhs;
}

bool parser::Parser::current_token_is(token::Kind kind) const {
    return _current_token.kind() == kind;
}

void parser::Parser::register_prefix(token::Kind kind, PrefixFn prefix_fn) {
    _prefix_parsers[kind] = std::move(prefix_fn);
}

void parser::Parser::register_infix(token::Kind kind, InfixFn infix_fn) {
    _infix_parsers[kind] = std::move(infix_fn);
}

std::unique_ptr<ast::IntegerLiteral> parser::Parser::parse_integer_literal() {
    return std::make_unique<ast::IntegerLiteral>(std::stoi(_current_token.literal()), _current_token);
}

std::unique_ptr<ast::ExpressionStatement> parser::Parser::parse_expression_statement() {
    auto token = _current_token;
    auto expression = parse_expression(Precedence::Lowest);
    return std::make_unique<ast::ExpressionStatement>(token, std::move(*expression));
}

ExpressionP parser::Parser::parse_identifier() {
    return std::make_unique<ast::Identifier>(_current_token, _current_token.literal());
}

parser::Precedence parser::Parser::peek_precedence() const {
    return precedence_for_token(_peek_token);
}

parser::Precedence parser::Parser::curr_precedence() const {
    return precedence_for_token(_current_token);
}

parser::Precedence parser::Parser::precedence_for_token(const token::Token &token) {
    auto pos = k_operator_precedence.find(token.kind());
    if (pos == k_operator_precedence.end()) {
        return Precedence::Lowest;
    }

    return pos->second;
}

ExpressionP parser::Parser::parse_infix_expression(ExpressionP expression) {
    auto precedence = curr_precedence();
    auto token = _current_token;
    next_token();
    auto rhs = parse_expression(precedence);
    return std::make_unique<ast::InfixExpression>(token, std::move(expression), std::move(*rhs), token.literal());
}
