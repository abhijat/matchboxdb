#ifndef MATCHBOXDB_COMMAND_EXECUTOR_H
#define MATCHBOXDB_COMMAND_EXECUTOR_H

#include <cstdint>
#include "sql_parser/statement_visitor.h"

namespace page_cache {
class PageCache;
}

namespace ast {
class Statement;
}

namespace command_executor {

struct EmptyResult {
};

using CommandExecutionResult = std::variant<uint32_t, std::vector<tuple::TupleView>, EmptyResult, std::string>;

class CommandExecutor : public ast::StatementVisitor {
public:
    explicit CommandExecutor(page_cache::PageCache &page_cache);

    CommandExecutionResult execute(const ast::Statement &statement);

    void visit(const ast::InsertStatement &insert_statement) override;

    void visit(const ast::DeleteStatement &delete_statement) override;

    void visit(const ast::CreateStatement &create_statement) override;

    void visit(const ast::UpdateStatement &update_statement) override;

    void visit(const ast::SelectStatement &select_statement) override;

    void visit(const ast::DropStatement &drop_statement) override;

    void visit(const ast::DescribeStatement &describe_statement) override;

private:
    page_cache::PageCache &_page_cache;
    CommandExecutionResult _command_execution_result;
};
} // namespace command_executor

#endif // MATCHBOXDB_COMMAND_EXECUTOR_H
