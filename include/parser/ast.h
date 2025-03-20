#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    VALUE_STRING,
    VALUE_NUMBER,
    VALUE_BOOL,
    VALUE_IP_ADDRESS,
    VALUE_IP_CIDR,
    VALUE_IP_RANGE,
    VALUE_IPV6_ADDRESS,
    VALUE_IPV6_CIDR,
    VALUE_IPV6_RANGE,
    VALUE_LIST
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        char* str_val;
        int int_val;
        int bool_val;
        struct Value** list_val;
    } data;
    int list_size; /* Only used for VALUE_LIST */
} Value;

typedef struct Property {
    char* name;
    Value* value;
    struct Property* next;
} Property;

typedef struct Block {
    char* name;
    Property* properties;
    struct Block* children;
    struct Block* next;
} Block;

typedef struct {
    Block* device;
    Block* interfaces;
    Block* ip;
    Block* routing;
    Block* firewall;
    Block* system;
} Configuration;

/* AST creation functions */
Value* create_string_value(char* str);
Value* create_number_value(int number);
Value* create_bool_value(int bool_val);
Value* create_ip_address_value(char* ip, ValueType type);
Value* create_list_value(Value** values, int size);

Property* create_property(char* name, Value* value);

Block* create_block(char* name);
void add_property_to_block(Block* block, Property* property);
void add_child_block(Block* parent, Block* child);

Configuration* create_configuration();
void add_section_to_configuration(Configuration* config, Block* section, char* section_name);

/* Semantic validation functions */
int validate_configuration(Configuration* config);

/* Memory management */
void free_value(Value* value);
void free_property(Property* property);
void free_block(Block* block);
void free_configuration(Configuration* config);

#endif /* AST_H */ 