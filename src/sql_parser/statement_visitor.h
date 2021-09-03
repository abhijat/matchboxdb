#ifndef MATCHBOXDB_STATEMENT_VISITOR_H
#define MATCHBOXDB_STATEMENT_VISITOR_H

#include <cstdint>

namespace ast {

class InsertStatement;

class DeleteStatement;

class CreateStatement;

class UpdateStatement;

class SelectStatement;

class StatementVisitor {
public:
    virtual uint32_t visit(const ast::InsertStatement &insert_statement) = 0;

    virtual void visit(const ast::DeleteStatement &insert_statement) = 0;

    virtual void visit(const ast::CreateStatement &insert_statement) = 0;

    virtual void visit(const ast::UpdateStatement &insert_statement) = 0;

    virtual void visit(const ast::SelectStatement &select_statement) = 0;
};

}

#endif //MATCHBOXDB_STATEMENT_VISITOR_H
