#include "ast.h"

/* AST creation functions */

Value* create_string_value(char* str) {
    Value* value = (Value*)malloc(sizeof(Value));
    value->type = VALUE_STRING;
    value->data.str_val = strdup(str);
    return value;
}

Value* create_number_value(int number) {
    Value* value = (Value*)malloc(sizeof(Value));
    value->type = VALUE_NUMBER;
    value->data.int_val = number;
    return value;
}

Value* create_bool_value(int bool_val) {
    Value* value = (Value*)malloc(sizeof(Value));
    value->type = VALUE_BOOL;
    value->data.bool_val = bool_val;
    return value;
}

Value* create_ip_address_value(char* ip, ValueType type) {
    Value* value = (Value*)malloc(sizeof(Value));
    value->type = type;
    value->data.str_val = strdup(ip);
    return value;
}

Value* create_list_value(Value** values, int size) {
    Value* value = (Value*)malloc(sizeof(Value));
    value->type = VALUE_LIST;
    value->data.list_val = values;
    value->list_size = size;
    return value;
}

Property* create_property(char* name, Value* value) {
    Property* property = (Property*)malloc(sizeof(Property));
    property->name = strdup(name);
    property->value = value;
    property->next = NULL;
    return property;
}

Block* create_block(char* name) {
    Block* block = (Block*)malloc(sizeof(Block));
    block->name = strdup(name);
    block->properties = NULL;
    block->children = NULL;
    block->next = NULL;
    return block;
}

void add_property_to_block(Block* block, Property* property) {
    if (!block->properties) {
        block->properties = property;
    } else {
        Property* current = block->properties;
        while (current->next) {
            current = current->next;
        }
        current->next = property;
    }
}

void add_child_block(Block* parent, Block* child) {
    if (!parent->children) {
        parent->children = child;
    } else {
        Block* current = parent->children;
        while (current->next) {
            current = current->next;
        }
        current->next = child;
    }
}

Configuration* create_configuration() {
    Configuration* config = (Configuration*)malloc(sizeof(Configuration));
    config->device = NULL;
    config->interfaces = NULL;
    config->ip = NULL;
    config->routing = NULL;
    config->firewall = NULL;
    config->system = NULL;
    return config;
}

void add_section_to_configuration(Configuration* config, Block* section, char* section_name) {
    if (strcmp(section_name, "device") == 0) {
        config->device = section;
    } else if (strcmp(section_name, "interfaces") == 0) {
        config->interfaces = section;
    } else if (strcmp(section_name, "ip") == 0) {
        config->ip = section;
    } else if (strcmp(section_name, "routing") == 0) {
        config->routing = section;
    } else if (strcmp(section_name, "firewall") == 0) {
        config->firewall = section;
    } else if (strcmp(section_name, "system") == 0) {
        config->system = section;
    }
}

/* Basic semantic validation - placeholder for more complex validation */
int validate_configuration(Configuration* config) {
    int valid = 1;

    /* Check required sections */
    if (!config->device) {
        fprintf(stderr, "Semantic error: Missing required 'device' section\n");
        valid = 0;
    }

    /* Add more validation rules as needed */

    return valid;
}

/* Memory management functions */

void free_value(Value* value) {
    if (!value) return;
    
    if (value->type == VALUE_STRING || 
        value->type == VALUE_IP_ADDRESS || 
        value->type == VALUE_IP_CIDR || 
        value->type == VALUE_IP_RANGE ||
        value->type == VALUE_IPV6_ADDRESS || 
        value->type == VALUE_IPV6_CIDR || 
        value->type == VALUE_IPV6_RANGE) {
        free(value->data.str_val);
    } else if (value->type == VALUE_LIST) {
        for (int i = 0; i < value->list_size; i++) {
            free_value(value->data.list_val[i]);
        }
        free(value->data.list_val);
    }
    
    free(value);
}

void free_property(Property* property) {
    if (!property) return;
    
    free(property->name);
    free_value(property->value);
    free_property(property->next);
    free(property);
}

void free_block(Block* block) {
    if (!block) return;
    
    free(block->name);
    free_property(block->properties);
    free_block(block->children);
    free_block(block->next);
    free(block);
}

void free_configuration(Configuration* config) {
    if (!config) return;
    
    free_block(config->device);
    free_block(config->interfaces);
    free_block(config->ip);
    free_block(config->routing);
    free_block(config->firewall);
    free_block(config->system);
    free(config);
} 