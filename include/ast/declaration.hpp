#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "ast_node_interface.hpp"
#include "statement.hpp"

// Base class for declarations
class Declaration : public ASTNodeInterface
{
public:
    Declaration(std::string_view decl_name) noexcept;
    
    const std::string& get_name() const noexcept;
    
protected:
    std::string name;
};

// Declaration for a configuration section
class ConfigDeclaration : public Declaration
{
public:
    ConfigDeclaration(std::string_view config_name) noexcept;
    ConfigDeclaration(std::string_view config_name, const StatementList& statements) noexcept;
    
    // Add a statement to this configuration
    void add_statement(Statement* statement) noexcept;
    
    const StatementList& get_statements() const noexcept;
    void destroy() noexcept override;
    std::string to_string() const override;
    
private:
    StatementList statements;
};

// Declaration for a property
class PropertyDeclaration : public Declaration
{
public:
    PropertyDeclaration(std::string_view prop_name, Expression* value) noexcept;
    
    Expression* get_value() const noexcept;
    void destroy() noexcept override;
    std::string to_string() const override;
    
private:
    Expression* value;
};

// Declaration for a network interface
class InterfaceDeclaration : public Declaration
{
public:
    InterfaceDeclaration(std::string_view iface_name) noexcept;
    InterfaceDeclaration(std::string_view iface_name, const StatementList& statements) noexcept;
    
    // Add a statement to this interface
    void add_statement(Statement* statement) noexcept;
    
    const StatementList& get_statements() const noexcept;
    void destroy() noexcept override;
    std::string to_string() const override;
    
private:
    StatementList statements;
};

// Program root declaration (holds all top-level sections)
class ProgramDeclaration : public Declaration
{
public:
    ProgramDeclaration() noexcept;
    
    // Add a section to this program
    void add_section(SectionStatement* section) noexcept;
    
    const std::vector<SectionStatement*>& get_sections() const noexcept;
    void destroy() noexcept override;
    std::string to_string() const override;
    
private:
    std::vector<SectionStatement*> sections;
}; 