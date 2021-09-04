#ifndef MATCHBOXDB_COMMAND_EXECUTOR_H
#define MATCHBOXDB_COMMAND_EXECUTOR_H

#include <cstdint>
#include "sql_parser/statement_visitor.h"

namespace page_cache {
class PageCache;
}

namespace ast {

class Statement;

class InsertStatement;

class DeleteStatement;

class CreateStatement;

class UpdateStatement;
} // namespace ast

namespace command_executor {

using CommandExecutionResult = std::variant<uint32_t, std::vector<tuple::Tuple>>;

class CommandExecutor : public ast::StatementVisitor {
public:
    explicit CommandExecutor(page_cache::PageCache& page_cache);

    CommandExecutionResult execute(const ast::Statement& statement);

    void visit(const ast::InsertStatement& insert_statement) override;

    void visit(const ast::DeleteStatement& delete_statement) override;

    void visit(const ast::CreateStatement& create_statement) override;

    void visit(const ast::UpdateStatement& update_statement) override;

    void visit(const ast::SelectStatement &select_statement) override;

private:
    page_cache::PageCache& _page_cache;
    CommandExecutionResult _command_execution_result;
};
} // namespace command_executor

#endif // MATCHBOXDB_COMMAND_EXECUTOR_H
