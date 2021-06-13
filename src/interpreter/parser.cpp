#include "parser.h"
#include "let_statement.h"

#include <utility>
#include <sstream>

parser::Parser::Parser(lexer::Lexer lexer) : _lexer(std::move(lexer)) {
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
        default:
            return std::nullopt;
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
