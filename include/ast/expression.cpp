#include "expression.hpp"
#include <sstream>

// Value implementation
Value::Value(ValueType val_type) noexcept : value_type(val_type) {}

Value::ValueType Value::get_value_type() const noexcept 
{
    return value_type;
}

void Value::destroy() noexcept 
{
    // No dynamic memory to clean up
}

// StringValue implementation
StringValue::StringValue(std::string_view str_value) noexcept 
    : Value(ValueType::STRING), str_value(str_value) {}

const std::string& StringValue::get_value() const noexcept 
{
    return str_value;
}

Datatype* StringValue::get_type() const 
{
    return new StringDatatype();
}

std::string StringValue::to_string() const 
{
    return "\"" + str_value + "\"";
}

// NumberValue implementation
NumberValue::NumberValue(int num_value) noexcept 
    : Value(ValueType::NUMBER), num_value(num_value) {}

int NumberValue::get_value() const noexcept 
{
    return num_value;
}

Datatype* NumberValue::get_type() const 
{
    return new NumberDatatype();
}

std::string NumberValue::to_string() const 
{
    return std::to_string(num_value);
}

// BooleanValue implementation
BooleanValue::BooleanValue(bool bool_value) noexcept 
    : Value(ValueType::BOOLEAN), bool_value(bool_value) {}

bool BooleanValue::get_value() const noexcept 
{
    return bool_value;
}

Datatype* BooleanValue::get_type() const 
{
    return new BooleanDatatype();
}

std::string BooleanValue::to_string() const 
{
    return bool_value ? "true" : "false";
}

// IPAddressValue implementation
IPAddressValue::IPAddressValue(std::string_view ip_value) noexcept 
    : Value(ValueType::IP_ADDRESS), ip_value(ip_value) {}

const std::string& IPAddressValue::get_value() const noexcept 
{
    return ip_value;
}

Datatype* IPAddressValue::get_type() const 
{
    return new IPAddressDatatype();
}

std::string IPAddressValue::to_string() const 
{
    return ip_value;
}

// IPCIDRValue implementation
IPCIDRValue::IPCIDRValue(std::string_view cidr_value) noexcept 
    : Value(ValueType::IP_CIDR), cidr_value(cidr_value) {}

const std::string& IPCIDRValue::get_value() const noexcept 
{
    return cidr_value;
}

Datatype* IPCIDRValue::get_type() const 
{
    return new IPCIDRDatatype();
}

std::string IPCIDRValue::to_string() const 
{
    return cidr_value;
}

// ListValue implementation
ListValue::ListValue(const ValueList& values, Datatype* element_type) noexcept 
    : values(values), element_type(element_type) {}

const ValueList& ListValue::get_values() const noexcept 
{
    return values;
}

void ListValue::destroy() noexcept 
{
    for (auto* value : values) {
        if (value) {
            value->destroy();
            delete value;
        }
    }
    values.clear();
    
    if (element_type) {
        element_type->destroy();
        delete element_type;
        element_type = nullptr;
    }
}

Datatype* ListValue::get_type() const 
{
    // If we have an element type, use it; otherwise try to determine from first element
    if (element_type) {
        return new ListDatatype(element_type->get_type() == Datatype::Type::LIST 
            ? new ListDatatype(static_cast<ListDatatype*>(element_type)->get_element_type())
            : element_type);
    }
    else if (!values.empty() && values[0]) {
        return new ListDatatype(values[0]->get_type());
    }
    
    // Default to list of strings if we can't determine
    return new ListDatatype(new StringDatatype());
}

std::string ListValue::to_string() const 
{
    std::stringstream ss;
    ss << "[";
    bool first = true;
    
    for (const auto* value : values) {
        if (!first) {
            ss << ", ";
        }
        if (value) {
            ss << value->to_string();
        } else {
            ss << "null";
        }
        first = false;
    }
    
    ss << "]";
    return ss.str();
}

// IdentifierExpression implementation
IdentifierExpression::IdentifierExpression(std::string_view name) noexcept 
    : name(name) {}

const std::string& IdentifierExpression::get_name() const noexcept 
{
    return name;
}

void IdentifierExpression::destroy() noexcept 
{
    // No dynamic memory to clean up
}

Datatype* IdentifierExpression::get_type() const 
{
    // This would typically be resolved during semantic analysis
    // Default to string type for now
    return new StringDatatype();
}

std::string IdentifierExpression::to_string() const 
{
    return name;
}

// PropertyReference implementation
PropertyReference::PropertyReference(Expression* base, std::string_view property_name) noexcept 
    : base(base), property_name(property_name) {}

const std::string& PropertyReference::get_property_name() const noexcept 
{
    return property_name;
}

Expression* PropertyReference::get_base() const noexcept 
{
    return base;
}

void PropertyReference::destroy() noexcept 
{
    if (base) {
        base->destroy();
        delete base;
        base = nullptr;
    }
}

Datatype* PropertyReference::get_type() const 
{
    // This would typically be resolved during semantic analysis
    // Default to string type for now
    return new StringDatatype();
}

std::string PropertyReference::to_string() const 
{
    return base ? base->to_string() + "." + property_name : property_name;
} 