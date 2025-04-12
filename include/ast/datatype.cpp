#include "datatype.hpp"

// Datatype implementation
Datatype::Datatype(Type type_value) noexcept : type(type_value) {}

Datatype::Type Datatype::get_type() const noexcept 
{
    return type; 
}

std::string Datatype::type_name() const 
{
    switch (type) {
        case Type::STRING: return "string";
        case Type::NUMBER: return "number";
        case Type::BOOLEAN: return "boolean";
        case Type::IP_ADDRESS: return "ip_address";
        case Type::IP_CIDR: return "ip_cidr";
        case Type::IP_RANGE: return "ip_range";
        case Type::IPV6_ADDRESS: return "ipv6_address";
        case Type::IPV6_CIDR: return "ipv6_cidr";
        case Type::IPV6_RANGE: return "ipv6_range";
        case Type::SECTION: return "section";
        case Type::LIST: return "list";
        default: return "unknown";
    }
}

std::string Datatype::to_string() const 
{
    return type_name();
}

// BasicDatatype implementation
BasicDatatype::BasicDatatype(Type type_value) noexcept : Datatype(type_value) {}

void BasicDatatype::destroy() noexcept 
{
    // No dynamic memory to clean up
}

// StringDatatype implementation
StringDatatype::StringDatatype() noexcept : BasicDatatype(Type::STRING) {}

// NumberDatatype implementation
NumberDatatype::NumberDatatype() noexcept : BasicDatatype(Type::NUMBER) {}

// BooleanDatatype implementation
BooleanDatatype::BooleanDatatype() noexcept : BasicDatatype(Type::BOOLEAN) {}

// IPAddressDatatype implementation
IPAddressDatatype::IPAddressDatatype() noexcept : BasicDatatype(Type::IP_ADDRESS) {}

// IPCIDRDatatype implementation
IPCIDRDatatype::IPCIDRDatatype() noexcept : BasicDatatype(Type::IP_CIDR) {}

// ConfigSectionDatatype implementation
ConfigSectionDatatype::ConfigSectionDatatype() noexcept : BasicDatatype(Type::SECTION) {}

std::string ConfigSectionDatatype::type_name() const {
    return "ConfigSection";
}

// InterfaceDatatype implementation
InterfaceDatatype::InterfaceDatatype() noexcept : BasicDatatype(Type::SECTION) {}

std::string InterfaceDatatype::type_name() const {
    return "Interface";
}

// ListDatatype implementation
ListDatatype::ListDatatype(Datatype* element_type) noexcept 
    : Datatype(Type::LIST), element_type(element_type) {}

void ListDatatype::destroy() noexcept 
{
    if (element_type) {
        element_type->destroy();
        delete element_type;
        element_type = nullptr;
    }
}

Datatype* ListDatatype::get_element_type() const noexcept 
{
    return element_type;
} 