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

std::string body_to_mikrotik(const Body& body, const std::string& ident) noexcept
{
    std::string result = "";
    for(Statement* statement : body)
    {
        result += statement->to_mikrotik(ident) + "\n";
    }
    return result;
}

// Virtual destructor implementation
ASTNodeInterface::~ASTNodeInterface() noexcept {} 