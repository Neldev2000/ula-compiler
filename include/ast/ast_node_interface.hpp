#pragma once

#include <forward_list>
#include <string>
#include <string_view>
#include <vector>

// Forward declarations of main node types
class Declaration;
class Expression;
class Statement;
class Datatype;
class ConfigSection;
class Property;
class Value;

// Type aliases for common structures
using StatementList = std::vector<Statement*>;
using PropertyList = std::vector<Property*>;
using ValueList = std::vector<Value*>;

// Helper function to destroy a list of statements
void destroy_statements(StatementList& statements) noexcept;

// Base interface for all AST nodes
class ASTNodeInterface
{
public:
    virtual ~ASTNodeInterface() noexcept;
    
    // Method to properly destroy the node and its children
    virtual void destroy() noexcept = 0;
    
    // Method to generate a string representation (useful for debugging)
    virtual std::string to_string() const = 0;
}; 