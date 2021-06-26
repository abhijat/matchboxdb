#include "parser.h"
#include "let_statement.h"
#include "return_statement.h"
#include "expression_statement.h"
#include "integer_literal.h"
#include "prefix_expression.h"
#include "infix_expression.h"
#include "boolean_expression.h"
#include "if_expression.h"
#include "function_expression.h"
#include "call_expression.h"

#include <utility>
#include <sstream>

parser::Parser::Parser(lexer::Lexer lexer) : _lexer(std::move(lexer)) {

    register_prefix_fn(token::TokenKind::IDENT, [&]() {
        return parse_identifier();
    });

    register_prefix_fn(token::TokenKind::INT, [&]() {
        return parse_integer_literal();
    });

    register_prefix_fn(token::TokenKind::BANG, [&]() {
        return parse_prefix_expression();
    });

    register_prefix_fn(token::TokenKind::MINUS, [&]() {
        return parse_prefix_expression();
    });

    register_prefix_fn(token::TokenKind::TRUE, [&]() {
        return parse_boolean_expression();
    });

    register_prefix_fn(token::TokenKind::FALSE, [&]() {
        return parse_boolean_expression();
    });

    register_prefix_fn(token::TokenKind::LPAREN, [&]() {
        return parse_grouped_expression();
    });

    register_prefix_fn(token::TokenKind::IF, [&]() {
        return parse_if_expression();
    });

    register_prefix_fn(token::TokenKind::FUNCTION, [&]() {
        return parse_function_literal();
    });

    auto infix_parser = [&](auto lhs) { return parse_infix_expression(std::move(lhs)); };

    register_infix_fn(token::TokenKind::PLUS, infix_parser);
    register_infix_fn(token::TokenKind::MINUS, infix_parser);
    register_infix_fn(token::TokenKind::SLASH, infix_parser);
    register_infix_fn(token::TokenKind::ASTERISK, infix_parser);
    register_infix_fn(token::TokenKind::GT, infix_parser);
    register_infix_fn(token::TokenKind::LT, infix_parser);
    register_infix_fn(token::TokenKind::EQ, infix_parser);
    register_infix_fn(token::TokenKind::NE, infix_parser);

    register_infix_fn(token::TokenKind::LPAREN, [&](auto lhs) {
        return parse_call_expression(std::move(lhs));
    });

    next_token();
    next_token();
}

void parser::Parser::next_token() {
    _current_token = _peek_token;
    _peek_token = _lexer.next_token();
}

ast::Program parser::Parser::parse() {
    std::vector<std::unique_ptr<ast::Statement>> statements;
    while (_current_token.kind != token::TokenKind::ENDOFINPUT) {
        auto statement = parse_statement();
        if (statement) {
            statements.push_back(std::move(*statement));
        }
        next_token();
    }
    return ast::Program{std::move(statements)};
}

std::optional<std::unique_ptr<ast::Statement>> parser::Parser::parse_statement() {
    switch (_current_token.kind) {
        case token::TokenKind::LET:
            return parse_let_statement();
        case token::TokenKind::RETURN:
            return parse_return_statement();
        default:
            return parse_expression_statement();
    }
}

std::optional<std::unique_ptr<ast::Statement>> parser::Parser::parse_let_statement() {
    auto token = _current_token;
    if (!expect_peek_token(token::TokenKind::IDENT)) {
        return std::nullopt;
    }

    auto name = ast::Identifier{_current_token, _current_token.literal};

    if (!expect_peek_token(token::TokenKind::ASSIGN)) {
        return std::nullopt;
    }

    while (!current_token_is(token::TokenKind::SEMICOLON)) {
        next_token();
    }

    return std::make_unique<ast::LetStatement>(token, name, nullptr);
}

bool parser::Parser::current_token_is(token::TokenKind token_kind) const {
    return _current_token.kind == token_kind;
}

bool parser::Parser::expect_peek_token(token::TokenKind token_kind) {
    if (_peek_token.kind == token_kind) {
        next_token();
        return true;
    } else {
        peek_error(token_kind);
        return false;
    }
}

const std::vector<std::string> &parser::Parser::errors() const {
    return _errors;
}

void parser::Parser::peek_error(token::TokenKind kind) {
    std::stringstream error;
    error << "expected next token to be " << kind << ", got " << _peek_token.kind << " instead";
    _errors.push_back(error.str());
}

std::optional<std::unique_ptr<ast::Statement>> parser::Parser::parse_return_statement() {
    auto token = _current_token;

    next_token();

    while (!current_token_is(token::TokenKind::SEMICOLON)) {
        next_token();
    }

    return std::make_unique<ast::ReturnStatement>(token, nullptr);
}

std::optional<std::unique_ptr<ast::Statement>> parser::Parser::parse_expression_statement() {
    auto token = _current_token;
    auto expression = parse_expression(Precedence::LOWEST);

    if (_peek_token.kind == token::TokenKind::SEMICOLON) {
        next_token();
    }

    return std::make_unique<ast::ExpressionStatement>(token, std::move(*expression));
}

std::optional<std::unique_ptr<ast::Expression>> parser::Parser::parse_expression(parser::Precedence precedence) {
    auto prefix_parser = _prefix_parsers.find(_current_token.kind);
    if (prefix_parser == _prefix_parsers.end()) {
        no_prefix_parser_fn_error(_current_token.kind);
        return std::nullopt;
    }

    auto lhs_expression = prefix_parser->second();

    while (_peek_token.kind != token::TokenKind::SEMICOLON && precedence < peek_precedence()) {
        auto infix_parser = _infix_parsers.find(_peek_token.kind);
        if (infix_parser == _infix_parsers.end()) {
            return lhs_expression;
        }

        next_token();
        lhs_expression = infix_parser->second(std::move(*lhs_expression));
    }

    return lhs_expression;
}

void parser::Parser::register_prefix_fn(token::TokenKind token_kind, parser::PrefixParserFn fn) {
    _prefix_parsers[token_kind] = std::move(fn);
}

void parser::Parser::register_infix_fn(token::TokenKind token_kind, parser::InfixParserFn fn) {
    _infix_parsers[token_kind] = std::move(fn);
}

std::optional<std::unique_ptr<ast::Expression>> parser::Parser::parse_identifier() {
    return std::make_unique<ast::Identifier>(_current_token, _current_token.literal);
}

std::optional<std::unique_ptr<ast::Expression>> parser::Parser::parse_integer_literal() {
    auto token = _current_token;
    auto value = std::stoi(_current_token.literal);
    return std::make_unique<ast::IntegerLiteral>(token, value);
}

void parser::Parser::no_prefix_parser_fn_error(token::TokenKind token_kind) {
    std::stringstream err;
    err << "no prefix parser function for " << token_kind << " found";
    _errors.push_back(err.str());
}

std::optional<std::unique_ptr<ast::Expression>> parser::Parser::parse_prefix_expression() {
    auto token = _current_token;
    next_token();
    auto expression = parse_expression(Precedence::PREFIX);
    return std::make_unique<ast::PrefixExpression>(token, token.literal, std::move(*expression));
}

parser::Precedence parser::Parser::peek_precedence() const {
    auto precedence = k_precedences.find(_peek_token.kind);
    if (precedence == k_precedences.end()) {
        return Precedence::LOWEST;
    }

    return precedence->second;
}

parser::Precedence parser::Parser::current_precedence() const {
    auto precedence = k_precedences.find(_current_token.kind);
    if (precedence == k_precedences.end()) {
        return Precedence::LOWEST;
    }

    return precedence->second;
}

std::optional<std::unique_ptr<ast::Expression>>
parser::Parser::parse_infix_expression(std::unique_ptr<ast::Expression> left) {
    auto token = _current_token;
    auto precedence = current_precedence();
    next_token();
    auto right = parse_expression(precedence);
    return std::make_unique<ast::InfixExpression>(token, std::move(left), std::move(*right), token.literal);
}

std::optional<std::unique_ptr<ast::Expression>> parser::Parser::parse_boolean_expression() {
    return std::make_unique<ast::BooleanExpression>(_current_token, current_token_is(token::TokenKind::TRUE));
}

std::optional<std::unique_ptr<ast::Expression>> parser::Parser::parse_grouped_expression() {
    next_token();
    auto expression = parse_expression(Precedence::LOWEST);
    if (!expect_peek_token(token::TokenKind::RPAREN)) {
        return std::nullopt;
    }
    return expression;
}

std::optional<std::unique_ptr<ast::Expression>> parser::Parser::parse_if_expression() {
    auto token = _current_token;
    if (!expect_peek_token(token::TokenKind::LPAREN)) {
        return std::nullopt;
    }

    next_token();
    auto condition = parse_expression(Precedence::LOWEST);
    if (!expect_peek_token(token::TokenKind::RPAREN)) {
        return std::nullopt;
    }

    if (!expect_peek_token(token::TokenKind::LBRACE)) {
        return std::nullopt;
    }

    auto consequence = parse_block_statement();

    std::optional<decltype(consequence)> alternative{};
    if (peek_token_is(token::TokenKind::ELSE)) {
        next_token();
        if (!expect_peek_token(token::TokenKind::LBRACE)) {
            return std::nullopt;
        }

        alternative = parse_block_statement();
    }

    return std::make_unique<ast::IfExpression>(std::move(token), std::move(*condition), std::move(consequence),
                                               std::move(alternative));
}

ast::BlockStatement parser::Parser::parse_block_statement() {
    auto token = _current_token;
    std::vector<std::unique_ptr<ast::Statement>> statements{};
    next_token();
    while (!current_token_is(token::TokenKind::RBRACE) && !current_token_is(token::TokenKind::ENDOFINPUT)) {
        auto statement = parse_statement();
        if (statement) {
            statements.push_back(std::move(*statement));
        }
        next_token();
    }
    return ast::BlockStatement{std::move(token), std::move(statements)};
}

std::optional<std::unique_ptr<ast::Expression>> parser::Parser::parse_function_literal() {
    auto token = _current_token;
    if (!expect_peek_token(token::TokenKind::LPAREN)) {
        return {};
    }

    auto params = parse_function_parameters();
    if (!expect_peek_token(token::TokenKind::LBRACE)) {
        return {};
    }

    return std::make_unique<ast::FunctionExpression>(std::move(token), std::move(params), parse_block_statement());
}

std::vector<ast::Identifier> parser::Parser::parse_function_parameters() {
    std::vector<ast::Identifier> params{};
    if (peek_token_is(token::TokenKind::RPAREN)) {
        next_token();
        return params;
    }

    next_token();
    params.emplace_back(_current_token, _current_token.literal);

    while (peek_token_is(token::TokenKind::COMMA)) {
        next_token();
        next_token();
        params.emplace_back(_current_token, _current_token.literal);
    }

    if (!expect_peek_token(token::TokenKind::RPAREN)) {
        return {};
    }

    return params;
}

bool parser::Parser::peek_token_is(token::TokenKind kind) const {
    return _peek_token.kind == kind;
}

std::optional<std::unique_ptr<ast::Expression>>
parser::Parser::parse_call_expression(std::unique_ptr<ast::Expression> left) {
    auto token = _current_token;
    return std::make_unique<ast::CallExpression>(std::move(token), std::move(left), parse_call_arguments());
}

std::vector<std::unique_ptr<ast::Expression>> parser::Parser::parse_call_arguments() {
    std::vector<std::unique_ptr<ast::Expression>> args{};
    if (peek_token_is(token::TokenKind::RPAREN)) {
        next_token();
        return args;
    }

    next_token();

    auto expr = parse_expression(Precedence::LOWEST);
    if (!expr) {
        return {};
    }

    args.push_back(std::move(*expr));

    while (peek_token_is(token::TokenKind::COMMA)) {
        next_token();
        next_token();
        expr = parse_expression(Precedence::LOWEST);
        if (!expr) {
            return {};
        }

        args.push_back(std::move(*expr));

    }

    if (!expect_peek_token(token::TokenKind::RPAREN)) {
        return {};
    }

    return args;
}
