#ifndef MATCHBOXDB_STATEMENT_VISITOR_H
#define MATCHBOXDB_STATEMENT_VISITOR_H

#include <cstdint>
#include <vector>
#include "../storage/tuple.h"

namespace ast {

class InsertStatement;

class DeleteStatement;

class CreateStatement;

class UpdateStatement;

class SelectStatement;

class DropStatement;

class DescribeStatement;

class StatementVisitor {
public:
    virtual void visit(const ast::InsertStatement &insert_statement) = 0;

    virtual void visit(const ast::DeleteStatement &insert_statement) = 0;

    virtual void visit(const ast::CreateStatement &insert_statement) = 0;

    virtual void visit(const ast::UpdateStatement &insert_statement) = 0;

    virtual void visit(const ast::SelectStatement &select_statement) = 0;

    virtual void visit(const ast::DropStatement &drop_statement) = 0;

    virtual void visit(const ast::DescribeStatement &describe_statement) = 0;
};

}

#endif //MATCHBOXDB_STATEMENT_VISITOR_H
