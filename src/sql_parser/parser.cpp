#include "parser.h"

#include "statement.h"
#include "select_statement.h"
#include "integer_literal.h"
#include "expression_statement.h"
#include "identifier.h"
#include "infix_expression.h"
#include "boolean_literal.h"
#include "field_definition.h"
#include "create_statement.h"
#include "update_statement.h"
#include "insert_statement.h"
#include "string_literal.h"
#include "delete_statement.h"
#include "drop_statement.h"
#include "describe_statement.h"
#include "parse_errors.h"

#include <utility>

parser::Parser::Parser(lexer::Lexer lexer)
    : _lexer(std::move(lexer)),
      _current_token(token::Kind::EndOfInput, ""),
      _peek_token(token::Kind::EndOfInput, "") {

    register_prefix(token::Kind::Integer, [&]() {
        return parse_integer_literal();
    });

    register_prefix(token::Kind::String, [&]() {
        return parse_string_literal();
    });

    register_prefix(token::Kind::Identifier, [&]() {
        return parse_identifier();
    });

    for (const auto kind: {
        token::Kind::True,
        token::Kind::False,
    }) {
        register_prefix(kind, [&]() {
            return parse_boolean_literal();
        });
    }

    register_prefix(token::Kind::LParen, [&]() {
        return parse_grouped_expression();
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
        case token::Kind::Create:
            return parse_create_statement();
        case token::Kind::Update:
            return parse_update_statement();
        case token::Kind::Insert:
            return parse_insert_statement();
        case token::Kind::Delete:
            return parse_delete_statement();
        case token::Kind::Drop:
            return parse_drop_statement();
        case token::Kind::Describe:
            return parse_describe_statement();
        default:
            return parse_expression_statement();
    }
}

std::unique_ptr<ast::SelectStatement> parser::Parser::parse_select_statement() {
    next_token();
    auto expressions = parse_expression_list();

    std::optional<ast::Table> table{};
    std::optional<std::unique_ptr<ast::Expression>> where_clause{};

    if (peek_token_is(token::Kind::From)) {
        // move to the FROM token
        next_token();

        // move to the actual table name
        next_token();

        table = parse_table_name();

        if (peek_token_is(token::Kind::Where)) {
            next_token();
            next_token();
            where_clause = parse_expression(Precedence::Lowest);
        }
    }

    return std::make_unique<ast::SelectStatement>(std::move(expressions), table, std::move(where_clause));
}

std::vector<std::unique_ptr<ast::Expression>> parser::Parser::parse_expression_list() {
    std::vector<std::unique_ptr<ast::Expression>> expressions{};
    auto expression = parse_expression(Precedence::Lowest);
    if (!expression) {
        throw parser::ParserError{"bad input: " + _current_token.literal()};
    }

    expressions.push_back(std::move(*expression));

    while (peek_token_is(token::Kind::Comma)) {

        // move to comma
        next_token();

        // move ahead of comma
        next_token();

        expression = parse_expression(Precedence::Lowest);
        if (!expression) {
            throw parser::ParserError{"bad input: " + _current_token.literal()};
        }

        expressions.push_back(std::move(*expression));
    }

    return expressions;
}

bool parser::Parser::peek_token_is(token::Kind kind) const {
    return _peek_token.kind() == kind;
}

std::optional<ast::Table> parser::Parser::parse_table_name() {
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
    return std::make_unique<ast::IntegerLiteral>(std::stoi(_current_token.literal()));
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

ExpressionP parser::Parser::parse_boolean_literal() {
    return std::make_unique<ast::BooleanLiteral>(_current_token.kind() == token::Kind::True);
}

ExpressionP parser::Parser::parse_grouped_expression() {
    next_token();
    auto expression = parse_expression(Precedence::Lowest);
    if (!expect_peek(token::Kind::RParen)) {
        return {};
    }

    return std::move(*expression);
}

bool parser::Parser::expect_peek(token::Kind kind) {
    if (peek_token_is(kind)) {
        next_token();
        return true;
    } else {
        peek_error(kind);
        throw parser::UnexpectedToken::unexpected_token(kind, _peek_token.kind());
        return false;
    }
}

void parser::Parser::peek_error(token::Kind kind) {
    std::stringstream ss;
    ss << "expected token kind " << kind << ", found token kind " << _peek_token.kind();
    _errors.push_back(ss.str());
}

std::unique_ptr<ast::Statement> parser::Parser::parse_create_statement() {
    // we start at CREATE, so move to the next token, which is the keyword TABLE
    next_token();

    // move from TABLE to the table name
    next_token();

    // read the table name
    auto table = parse_table_name();

    if (!table) {
        throw parser::ParserError{"bad table name"};
    }

    // move up to the (
    if (!expect_peek(token::Kind::LParen)) {
        throw parser::ParserError{"no lparen after table name"};
    }

    // move over to the field definitions, extract fields
    next_token();
    auto field_definitions = parse_field_definitions();

    if (!expect_peek(token::Kind::RParen)) {
        throw parser::ParserError{"no rparen after fields"};
    }

    return std::make_unique<ast::CreateStatement>(*table, field_definitions);
}

std::vector<ast::FieldDefinition> parser::Parser::parse_field_definitions() {
    // parses a series of field definitions
    std::vector<ast::FieldDefinition> field_definitions{};

    // the first field, before any commas
    auto field_definition = parse_field_definition();

    if (!field_definition) {
        throw parser::ParserError{"bad field definition: " + _current_token.literal()};
    }

    field_definitions.push_back(*field_definition);

    // the next token, if it is a comma, means there are more fields up ahead.
    while (peek_token_is(token::Kind::Comma)) {
        // move over the comma
        next_token();
        next_token();

        // read the next field. once it is consumed, the next token is either a comma, or an RParen
        field_definition = parse_field_definition();

        if (!field_definition) {
            throw parser::ParserError{"bad field definition: " + _current_token.literal()};
        }

        field_definitions.push_back(*field_definition);
    }

    return field_definitions;
}

std::optional<ast::FieldDefinition> parser::Parser::parse_field_definition() {
    // parses a single field definition, such as:
    // ... username STRING, ...
    auto field_name = _current_token.literal();
    next_token();
    return ast::FieldDefinition{field_name, _current_token.kind()};
}

std::unique_ptr<ast::Statement> parser::Parser::parse_update_statement() {
    // move to the table name
    next_token();

    auto table = parse_table_name();
    if (!table) {
        throw parser::ParserError{"bad table: " + _current_token.literal()};
    }

    expect_peek(token::Kind::Set);

    next_token();

    std::map<ast::Identifier, std::unique_ptr<ast::Expression>> updates;
    std::vector<ast::Identifier> insertion_order;

    auto pair = parse_field_update();
    insertion_order.push_back(pair.first);
    updates.emplace(std::move(pair));

    while (peek_token_is(token::Kind::Comma)) {
        next_token();
        next_token();
        pair = parse_field_update();
        insertion_order.push_back(pair.first);
        updates.emplace(std::move(pair));

    }

    auto where = std::optional<ExpressionP>{};
    if (peek_token_is(token::Kind::Where)) {
        next_token();
        next_token();

        where = parse_expression(Precedence::Lowest);
    }

    return std::make_unique<ast::UpdateStatement>(*table, std::move(updates), std::move(where),
                                                  std::move(insertion_order));
}

std::pair<ast::Identifier, std::unique_ptr<ast::Expression>> parser::Parser::parse_field_update() {
    auto identifier = ast::Identifier{_current_token, _current_token.literal()};
    expect_peek(token::Kind::Equals);

    next_token();
    auto expression = parse_expression(Precedence::Lowest);
    if (!expression) {
        throw parser::ParserError{"bad expression: " + _current_token.literal()};
    }

    return {identifier, std::move(*expression)};
}

std::unique_ptr<ast::Statement> parser::Parser::parse_insert_statement() {
    expect_peek(token::Kind::Into);
    next_token();
    auto table = parse_table_name();
    if (!table) {
        throw parser::ParserError{"Failed to parse table name"};
    }

    expect_peek(token::Kind::Values);

    expect_peek(token::Kind::LParen);

    next_token();

    auto expressions = parse_expression_list();

    expect_peek(token::Kind::RParen);

    return std::make_unique<ast::InsertStatement>(std::move(*table), std::move(expressions));
}

ExpressionP parser::Parser::parse_string_literal() {
    return std::make_unique<ast::StringLiteral>(_current_token.literal());
}

std::unique_ptr<ast::Statement> parser::Parser::parse_delete_statement() {
    expect_peek(token::Kind::From);

    next_token();
    auto table = parse_table_name();
    if (!table) {
        throw parser::ParserError{"bad table name: " + _current_token.literal()};
    }

    std::optional<std::unique_ptr<ast::Expression>> where{};
    if (peek_token_is(token::Kind::Where)) {
        next_token();
        next_token();

        where = parse_expression(Precedence::Lowest);
    }

    return std::make_unique<ast::DeleteStatement>(std::move(*table), std::move(where));
}

std::unique_ptr<ast::Statement> parser::Parser::parse_drop_statement() {
    expect_peek(token::Kind::Table);

    next_token();

    auto table = parse_table_name();
    if (!table) {
        throw parser::ParserError{"bad table name: " + _current_token.literal()};
    }

    return std::make_unique<ast::DropStatement>(std::move(*table));
}

std::unique_ptr<ast::Statement> parser::Parser::parse_describe_statement() {
    expect_peek(token::Kind::Table);

    next_token();

    if (current_token_is(token::Kind::Semicolon) || current_token_is(token::Kind::EndOfInput)) {
        return std::make_unique<ast::DescribeStatement>(std::nullopt);
    } else {
        return std::make_unique<ast::DescribeStatement>(parse_table_name());
    }
}
