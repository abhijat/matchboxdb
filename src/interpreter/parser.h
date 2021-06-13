#ifndef MATCHBOXDB_PARSER_H
#define MATCHBOXDB_PARSER_H

#include "lexer.h"
#include "program.h"

namespace parser {

class Parser {
public:
    Parser(lexer::Lexer lexer);

    ast::Program parse();

    const std::vector<std::string> &errors() const;

protected:
    std::optional<std::unique_ptr<ast::Statement>> parse_statement();

    std::optional<std::unique_ptr<ast::Statement>> parse_let_statement();

    void next_token();

    bool current_token_is(token::TokenKind token_kind) const;

    bool expect_peek_token(token::TokenKind token_kind);

    void peek_error(token::TokenKind expected);

protected:
    lexer::Lexer _lexer;
    token::Token _current_token{};
    token::Token _peek_token{};
    std::vector<std::string> _errors{};
};

}


#endif //MATCHBOXDB_PARSER_H
