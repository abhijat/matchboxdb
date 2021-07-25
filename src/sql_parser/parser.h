#ifndef MATCHBOXDB_PARSER_H
#define MATCHBOXDB_PARSER_H

#include <memory>
#include "lexer.h"
#include "table.h"
#include "identifier.h"

namespace ast {
class Statement;

class SelectStatement;

class Expression;

class IntegerLiteral;

class ExpressionStatement;

class FieldDefinition;
}

namespace {

using ExpressionP = std::unique_ptr<ast::Expression>;

using PrefixFn = std::function<ExpressionP()>;

using InfixFn = std::function<ExpressionP(ExpressionP)>;

}

namespace parser {

enum class Precedence {
    Lowest,
    LessGreater,
    Or,
    And,
    Equals,
    Prefix,
};

static const std::map<token::Kind, Precedence> k_operator_precedence{
    {token::Kind::Equals, Precedence::Equals},
    {token::Kind::NE,     Precedence::Equals},
    {token::Kind::LT,     Precedence::LessGreater},
    {token::Kind::GT,     Precedence::LessGreater},
    {token::Kind::And,    Precedence::And},
    {token::Kind::Or,     Precedence::Or},
};

class Parser {
public:
    explicit Parser(lexer::Lexer lexer);

    std::unique_ptr<ast::Statement> parse();


protected:
    void next_token();

protected:
    lexer::Lexer _lexer;
    token::Token _current_token;
    token::Token _peek_token;

    std::map<token::Kind, PrefixFn> _prefix_parsers{};
    std::map<token::Kind, InfixFn> _infix_parsers{};

    std::vector<std::string> _errors{};

    void peek_error(token::Kind kind);

    void register_prefix(token::Kind kind, PrefixFn prefix_fn);

    void register_infix(token::Kind kind, InfixFn infix_fn);

    std::unique_ptr<ast::Statement> parse_statement();

    std::unique_ptr<ast::SelectStatement> parse_select_statement();

    std::vector<std::unique_ptr<ast::Expression>> parse_expression_list();

    [[nodiscard]] bool peek_token_is(token::Kind kind) const;

    [[nodiscard]] bool current_token_is(token::Kind kind) const;

    std::optional<ast::Table> parse_table_name();

    std::optional<std::unique_ptr<ast::Expression>> parse_expression(Precedence precedence);

    std::unique_ptr<ast::IntegerLiteral> parse_integer_literal();

    std::unique_ptr<ast::ExpressionStatement> parse_expression_statement();

    ExpressionP parse_identifier();

    [[nodiscard]] Precedence peek_precedence() const;

    [[nodiscard]] Precedence curr_precedence() const;

    [[nodiscard]] static Precedence precedence_for_token(const token::Token &token);

    ExpressionP parse_infix_expression(ExpressionP expression);

    ExpressionP parse_boolean_literal();

    ExpressionP parse_grouped_expression();

    bool expect_peek(token::Kind kind);

    std::unique_ptr<ast::Statement> parse_create_statement();

    std::vector<ast::FieldDefinition> parse_field_definitions();

    std::optional<ast::FieldDefinition> parse_field_definition();

    std::unique_ptr<ast::Statement> parse_update_statement();

    std::pair<ast::Identifier, std::unique_ptr<ast::Expression>> parse_field_update();

    std::unique_ptr<ast::Statement> parse_insert_statement();

    ExpressionP parse_string_literal();
};

}

#endif //MATCHBOXDB_PARSER_H
