#include "expressions.hpp"
#include <sstream>

// ConfigNode implementation
ConfigNode::~ConfigNode() {}

// Value implementation
Value::Value(const std::string& val, ValueType type) noexcept
    : value{val}, type{type} {}

void Value::destroy() noexcept {}

std::string Value::to_string() const noexcept {
    return value;
}

std::string Value::get_value() const noexcept {
    return value;
}

ValueType Value::get_type() const noexcept {
    return type;
}

// ListValue implementation
ListValue::ListValue() noexcept {}

void ListValue::add_value(Value* val) noexcept {
    values.push_back(val);
}

void ListValue::destroy() noexcept {
    for (auto* val : values) {
        val->destroy();
        delete val;
    }
    values.clear();
}

std::string ListValue::to_string() const noexcept {
    std::ostringstream oss;
    oss << "[ ";
    
    bool first = true;
    for (const auto* val : values) {
        if (!first) {
            oss << ", ";
        }
        oss << val->to_string();
        first = false;
    }
    
    oss << " ]";
    return oss.str();
}

// Property implementation
Property::Property(const std::string& name, ConfigNode* val) noexcept
    : name{name}, value{val} {}

void Property::destroy() noexcept {
    if (value) {
        value->destroy();
        delete value;
        value = nullptr;
    }
}

std::string Property::to_string() const noexcept {
    return name + " = " + value->to_string();
}

std::string Property::get_name() const noexcept {
    return name;
}

ConfigNode* Property::get_value() const noexcept {
    return value;
}

// Block implementation
Block::Block() noexcept {}

void Block::add_statement(ConfigNode* statement) noexcept {
    statements.push_back(statement);
}

void Block::destroy() noexcept {
    for (auto* stmt : statements) {
        stmt->destroy();
        delete stmt;
    }
    statements.clear();
}

std::string Block::to_string() const noexcept {
    std::ostringstream oss;
    oss << "{\n";
    
    for (const auto* stmt : statements) {
        oss << "    " << stmt->to_string() << "\n";
    }
    
    oss << "}";
    return oss.str();
}

const std::vector<ConfigNode*>& Block::get_statements() const noexcept {
    return statements;
}

// Section implementation
Section::Section(const std::string& name, SectionType type, Block* block) noexcept
    : name{name}, type{type}, block{block} {}

void Section::destroy() noexcept {
    if (block) {
        block->destroy();
        delete block;
        block = nullptr;
    }
}

std::string Section::to_string() const noexcept {
    return name + ": " + block->to_string();
}

std::string Section::get_name() const noexcept {
    return name;
}

SectionType Section::get_type() const noexcept {
    return type;
}

Block* Section::get_block() const noexcept {
    return block;
}

// Configuration implementation
Configuration::Configuration() noexcept {}

void Configuration::add_section(Section* section) noexcept {
    sections.push_back(section);
}

void Configuration::destroy() noexcept {
    for (auto* section : sections) {
        section->destroy();
        delete section;
    }
    sections.clear();
}

std::string Configuration::to_string() const noexcept {
    std::ostringstream oss;
    
    for (const auto* section : sections) {
        oss << section->to_string() << "\n\n";
    }
    
    return oss.str();
} 