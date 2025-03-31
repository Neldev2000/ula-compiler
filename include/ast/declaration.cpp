#include "declaration.hpp"
#include <sstream>

// Declaration implementation
Declaration::Declaration(std::string_view decl_name) noexcept : name(decl_name) {}

const std::string& Declaration::get_name() const noexcept 
{
    return name;
}

// ConfigDeclaration implementation
ConfigDeclaration::ConfigDeclaration(std::string_view config_name) noexcept 
    : Declaration(config_name), statements() {}

ConfigDeclaration::ConfigDeclaration(std::string_view config_name, const StatementList& statements) noexcept 
    : Declaration(config_name), statements(statements) {}

void ConfigDeclaration::add_statement(Statement* statement) noexcept 
{
    if (statement) {
        statements.push_back(statement);
    }
}

const StatementList& ConfigDeclaration::get_statements() const noexcept 
{
    return statements;
}

void ConfigDeclaration::destroy() noexcept 
{
    for (auto* statement : statements) {
        if (statement) {
            statement->destroy();
            delete statement;
        }
    }
    statements.clear();
}

std::string ConfigDeclaration::to_string() const 
{
    std::stringstream ss;
    ss << name << ":\n";
    for (const auto* statement : statements) {
        if (statement) {
            ss << "    " << statement->to_string() << "\n";
        }
    }
    return ss.str();
}

// PropertyDeclaration implementation
PropertyDeclaration::PropertyDeclaration(std::string_view prop_name, Expression* value) noexcept 
    : Declaration(prop_name), value(value) {}

Expression* PropertyDeclaration::get_value() const noexcept 
{
    return value;
}

void PropertyDeclaration::destroy() noexcept 
{
    if (value) {
        value->destroy();
        delete value;
        value = nullptr;
    }
}

std::string PropertyDeclaration::to_string() const 
{
    std::stringstream ss;
    ss << name << " = ";
    if (value) {
        ss << value->to_string();
    } else {
        ss << "null";
    }
    return ss.str();
}

// InterfaceDeclaration implementation
InterfaceDeclaration::InterfaceDeclaration(std::string_view iface_name) noexcept 
    : Declaration(iface_name), statements() {}

InterfaceDeclaration::InterfaceDeclaration(std::string_view iface_name, const StatementList& statements) noexcept 
    : Declaration(iface_name), statements(statements) {}

void InterfaceDeclaration::add_statement(Statement* statement) noexcept 
{
    if (statement) {
        statements.push_back(statement);
    }
}

const StatementList& InterfaceDeclaration::get_statements() const noexcept 
{
    return statements;
}

void InterfaceDeclaration::destroy() noexcept 
{
    for (auto* statement : statements) {
        if (statement) {
            statement->destroy();
            delete statement;
        }
    }
    statements.clear();
}

std::string InterfaceDeclaration::to_string() const 
{
    std::stringstream ss;
    ss << name << ":\n";
    for (const auto* statement : statements) {
        if (statement) {
            ss << "    " << statement->to_string() << "\n";
        }
    }
    return ss.str();
}

// ProgramDeclaration implementation
ProgramDeclaration::ProgramDeclaration() noexcept 
    : Declaration("program"), sections() {}

void ProgramDeclaration::add_section(SectionStatement* section) noexcept 
{
    if (section) {
        sections.push_back(section);
    }
}

const std::vector<SectionStatement*>& ProgramDeclaration::get_sections() const noexcept 
{
    return sections;
}

void ProgramDeclaration::destroy() noexcept 
{
    for (auto* section : sections) {
        if (section) {
            section->destroy();
            delete section;
        }
    }
    sections.clear();
}

std::string ProgramDeclaration::to_string() const 
{
    std::stringstream ss;
    for (const auto* section : sections) {
        if (section) {
            ss << section->to_string() << "\n";
        }
    }
    return ss.str();
} 