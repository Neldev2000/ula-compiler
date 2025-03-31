#include "ast_node_interface.hpp"
#include "statement.hpp"

// Implementation of helper function to destroy a list of statements
void destroy_statements(StatementList& statements) noexcept
{
    for (auto* statement : statements)
    {
        if (statement)
        {
            statement->destroy();
            delete statement;
        }
    }
    statements.clear();
}

// Virtual destructor implementation
ASTNodeInterface::~ASTNodeInterface() noexcept {} 