#pragma once

#include <string>
#include <vector>
#include <memory>

// Forward declarations
class ConfigNode;
class Section;
class Property;
class Value;
class Block;

// Value types
enum class ValueType {
    STRING,
    NUMBER,
    BOOLEAN,
    IP_ADDRESS,
    IP_CIDR,
    IP_RANGE,
    IPV6_ADDRESS,
    IPV6_CIDR,
    IPV6_RANGE,
    KEYWORD
};

// Section types
enum class SectionType {
    NONE,
    DEVICE,
    INTERFACES,
    IP,
    ROUTING,
    FIREWALL,
    SYSTEM
};

// Base class for all configuration nodes
class ConfigNode {
public:
    virtual ~ConfigNode();
    virtual void destroy() noexcept = 0;
    virtual std::string to_string() const noexcept = 0;
};

// Represents a configuration value
class Value : public ConfigNode {
public:
    Value(const std::string& val, ValueType type) noexcept;
    
    void destroy() noexcept override;
    std::string to_string() const noexcept override;
    
    std::string get_value() const noexcept;
    ValueType get_type() const noexcept;

private:
    std::string value;
    ValueType type;
};

// Represents a list of values
class ListValue : public ConfigNode {
public:
    ListValue() noexcept;
    
    void add_value(Value* val) noexcept;
    void destroy() noexcept override;
    std::string to_string() const noexcept override;

private:
    std::vector<Value*> values;
};

// Represents a property with name and value
class Property : public ConfigNode {
public:
    Property(const std::string& name, ConfigNode* val) noexcept;
    
    void destroy() noexcept override;
    std::string to_string() const noexcept override;
    
    std::string get_name() const noexcept;
    ConfigNode* get_value() const noexcept;

private:
    std::string name;
    ConfigNode* value;
};

// Represents a block of statements
class Block : public ConfigNode {
public:
    Block() noexcept;
    
    void add_statement(ConfigNode* statement) noexcept;
    void destroy() noexcept override;
    std::string to_string() const noexcept override;
    
    const std::vector<ConfigNode*>& get_statements() const noexcept;

private:
    std::vector<ConfigNode*> statements;
};

// Represents a section in the configuration
class Section : public ConfigNode {
public:
    Section(const std::string& name, SectionType type, Block* block) noexcept;
    
    void destroy() noexcept override;
    std::string to_string() const noexcept override;
    
    std::string get_name() const noexcept;
    SectionType get_type() const noexcept;
    Block* get_block() const noexcept;

private:
    std::string name;
    SectionType type;
    Block* block;
};

// Represents the entire configuration
class Configuration : public ConfigNode {
public:
    Configuration() noexcept;
    
    void add_section(Section* section) noexcept;
    void destroy() noexcept override;
    std::string to_string() const noexcept override;

private:
    std::vector<Section*> sections;
}; 