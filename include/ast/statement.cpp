#include "statement.hpp"
#include "declaration.hpp"
#include <sstream>

// PropertyStatement implementation
PropertyStatement::PropertyStatement(std::string_view name, Expression* value) noexcept 
    : name(name), value(value) {}

const std::string& PropertyStatement::get_name() const noexcept 
{
    return name;
}

Expression* PropertyStatement::get_value() const noexcept 
{
    return value;
}

void PropertyStatement::destroy() noexcept 
{
    if (value) {
        value->destroy();
        delete value;
        value = nullptr;
    }
}

std::string PropertyStatement::to_string() const 
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

// BlockStatement implementation
BlockStatement::BlockStatement() noexcept : statements() {}

BlockStatement::BlockStatement(const StatementList& statements) noexcept 
    : statements(statements) {}

void BlockStatement::add_statement(Statement* statement) noexcept 
{
    if (statement) {
        statements.push_back(statement);
    }
}

const StatementList& BlockStatement::get_statements() const noexcept 
{
    return statements;
}

void BlockStatement::destroy() noexcept 
{
    for (auto* statement : statements) {
        if (statement) {
            statement->destroy();
            delete statement;
        }
    }
    statements.clear();
}

std::string BlockStatement::to_string() const 
{
    std::stringstream ss;
    for (const auto* statement : statements) {
        if (statement) {
            ss << "    " << statement->to_string() << "\n";
        }
    }
    return ss.str();
}

// SectionStatement implementation
SectionStatement::SectionStatement(std::string_view name, SectionType type) noexcept 
    : name(name), type(type), block(nullptr) {}

SectionStatement::SectionStatement(std::string_view name, SectionType type, BlockStatement* block) noexcept 
    : name(name), type(type), block(block) {}

const std::string& SectionStatement::get_name() const noexcept 
{
    return name;
}

SectionStatement::SectionType SectionStatement::get_section_type() const noexcept 
{
    return type;
}

BlockStatement* SectionStatement::get_block() const noexcept 
{
    return block;
}

void SectionStatement::set_block(BlockStatement* block) noexcept 
{
    this->block = block;
}

std::string SectionStatement::section_type_to_string(SectionType type) 
{
    switch (type) {
        case SectionType::DEVICE: return "device";
        case SectionType::INTERFACES: return "interfaces";
        case SectionType::IP: return "ip";
        case SectionType::ROUTING: return "routing";
        case SectionType::FIREWALL: return "firewall";
        case SectionType::SYSTEM: return "system";
        case SectionType::CUSTOM: return "custom";
        default: return "unknown";
    }
}

void SectionStatement::destroy() noexcept 
{
    if (block) {
        block->destroy();
        delete block;
        block = nullptr;
    }
}

std::string SectionStatement::to_string() const 
{
    std::stringstream ss;
    ss << name << ":\n";
    if (block) {
        ss << block->to_string();
    }
    return ss.str();
}

// DeclarationStatement implementation
DeclarationStatement::DeclarationStatement(Declaration* decl) noexcept 
    : declaration(decl) {}

Declaration* DeclarationStatement::get_declaration() const noexcept 
{
    return declaration;
}

void DeclarationStatement::destroy() noexcept 
{
    if (declaration) {
        declaration->destroy();
        delete declaration;
        declaration = nullptr;
    }
}

std::string DeclarationStatement::to_string() const 
{
    return declaration ? declaration->to_string() : "null";
} 