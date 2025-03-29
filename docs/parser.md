# Parsing the DSL Language for Mikrotik Network Configuration

## What is the Parser?

The parser is a component that reads the structured text of our Mikrotik DSL (Domain Specific Language) and checks if it follows the rules of the language. Think of it as a proofreader that ensures your configuration follows the correct syntax.

If the syntax is valid, the parser accepts the configuration. If there are syntax errors, the parser will reject it and provide helpful error messages explaining what went wrong.

## Our Parsing Approach

We use a technique called **LALR(1)** (Look-Ahead LR) parsing for this DSL. Here's what that means in simple terms:

- **Bottom-up parsing**: The parser reads the input from left to right and builds the structure from the smallest pieces up to larger ones.
- **Context awareness**: It uses one token of "look-ahead" to make decisions about how to interpret the current part of the configuration.
- **Efficient processing**: LALR(1) offers a good balance between power (what it can recognize) and performance (how fast it works).

We chose LALR(1) because it's well-suited for hierarchical configurations like our DSL, where sections can contain other sections and properties. It's also efficient enough to handle large configuration files without performance issues.

### Why LALR(1) and Not Other Grammar Types?

We specifically chose LALR(1) over other common grammar types for several technical reasons:

#### Comparison with LL Grammars (LL(0), LL(1))

- **LL(0)** parsers, also known as predictive parsers without lookahead, are too restrictive for our DSL. They cannot handle ambiguities that naturally arise in network configuration syntax, such as distinguishing between different types of property assignments or nested sections without looking ahead.

- **LL(1)** parsers (top-down parsers with one token lookahead) would require excessive grammar transformations to handle our DSL:
  - Left-recursion elimination would be necessary for expressions like nested sections
  - The left-factoring required would make the grammar less intuitive and harder to maintain
  - Our DSL has constructs that are inherently easier to parse bottom-up than top-down

#### Comparison with Full LR(1)

- **LR(1)** parsers are more powerful than LALR(1) but at a cost:
  - They generate much larger parsing tables (often exponentially larger)
  - They require more memory and can be slower to execute
  - The additional power of LR(1) isn't necessary for our DSL's grammar

#### Why LALR(1) is the Right Choice

- **Practical implementation**: Using Bison (which implements LALR(1)) gives us a robust, well-tested parser generator
- **Efficient parsing**: LALR(1) parsers have compact parse tables compared to LR(1)
- **Sufficient power**: LALR(1) can handle the entire range of our DSL syntax without limitation
- **Error handling**: Provides better error diagnostics than LL parsers, allowing us to give users clear feedback on syntax errors
- **Handles ambiguity**: Can resolve common ambiguities in network configuration syntax with minimal grammar complexity

LALR(1) strikes the optimal balance for our DSL between parser complexity, runtime efficiency, and grammar expressiveness.

The parser keeps track of the current section (device, interfaces, etc.) and nesting level as it processes the configuration, which helps it understand the context of each statement.

## DSL Language Basics

Our DSL is designed to configure Mikrotik network devices in a human-readable format. Instead of using complex command-line instructions, the DSL uses a straightforward hierarchical structure:

### Key Syntax Rules:

1. **Configuration is organized into sections** (like `device:`, `interfaces:`, etc.)
2. **Properties are assigned using equals signs** (`vendor = "mikrotik"`)
3. **Hierarchical structure is created using colons** (`ether1:`)
4. **No semicolons are allowed** at the end of statements
5. **Indentation is used** for readability (but not enforced by the parser)

### Valid and Invalid Syntax Examples

#### ✅ Valid syntax:
```
device:
    vendor = "mikrotik"
    model = "CCR2004-1G-12S+2XS"
```

#### ❌ Invalid syntax:
```
device:
    vendor = "mikrotik";  # Semicolons are not allowed
```

## Main Sections of the DSL

The DSL supports six main section types:

1. `device`: Basic device information
2. `interfaces`: Network interface configuration
3. `ip`: IP addressing and services
4. `routing`: Routing protocols and static routes
5. `firewall`: Firewall rules and NAT
6. `system`: System-level configuration

## Grammar Structure Explained

### Sections

A section is the top-level building block of the configuration:

```
device:
    # properties and subsections go here
```

### Properties

Properties are key-value pairs that define configuration attributes:

```
vendor = "mikrotik"
model = "CCR2004"
enabled = true
port = 123
address = 192.168.1.1/24
```

### Nested Blocks

Blocks can be nested to create hierarchical configurations:

```
interfaces:
    ether1:
        type = "ethernet"
        ethernet:
            speed = "1Gbps"
```

### Lists

Values can also be lists, enclosed in square brackets:

```
firewall:
    filter:
        connection_state = ["established", "related"]
```

## Common Syntax Errors

Our parser provides clear error messages for common syntax issues:

1. **Using semicolons**: Semicolons are not allowed in this DSL.
   ```
   vendor = "mikrotik";  # ERROR: Semicolons are not allowed in this DSL
   ```

2. **Invalid tokens after colon**: After a section declaration, only a newline or comment is allowed.
   ```
   interfaces: ;  # ERROR: Invalid syntax after colon
   ```

3. **Invalid property assignment**: 
   ```
   vendor $ "mikrotik"  # ERROR: Invalid property assignment syntax
   ```

4. **Unknown tokens or invalid syntax**:
   ```
   device: @  # ERROR: Unknown token or invalid syntax
   ```

## Handling Comments

The DSL supports two types of comments:

1. **Single-line comments** starting with `#`:
   ```
   # This is a comment
   device:  # This is an end-of-line comment
   ```

2. **Multi-line comments** enclosed in triple quotes `"""`:
   ```
   """
   This is a multi-line comment
   that spans multiple lines
   """
   ```

## Example Configurations

### Basic Device Setup

```
device:
    vendor = "mikrotik"
    model = "CCR2004-1G-12S+2XS"
```

### Interface Configuration

```
interfaces:
    ether1:
        type = "ethernet"
        admin_state = "enabled"
        description = "WAN Connection"
```

### Firewall Rule

```
firewall:
    filter:
        input_accept_established:
            chain = "input"
            connection_state = ["established", "related"]
            action = "accept"
```

## Class-Based Configuration Representation

Our parser implementation uses a robust object-oriented approach to represent and manipulate network configurations through the `expressions.hpp` and `expressions.cpp` files.

### Configuration Class Hierarchy

The implementation follows a hierarchical class structure with `ConfigNode` as the base class:

- **ConfigNode (Abstract Base Class)**:
  - Defines the interface for all configuration components
  - Provides virtual methods for resource cleanup and string representation
  - Ensures polymorphic behavior throughout the configuration tree

- **Value**:
  - Represents primitive values in the configuration (strings, numbers, booleans, IP addresses)
  - Each value has both a content and a type (ValueType enum)
  - Types include STRING, NUMBER, BOOLEAN, IP_ADDRESS, IP_CIDR, and others

- **ListValue**:
  - Represents lists of values, such as multiple IP addresses or firewall rules
  - Maintains and manages a vector of Value objects
  - Provides iteration and access to the contained values

- **Property**:
  - Represents a configuration property with a name and a value
  - Example: `vendor = "mikrotik"` or `admin_state = "enabled"`
  - Value can be a simple Value or a more complex ListValue

- **Block**:
  - Represents a collection of configuration statements (properties or subsections)
  - Maintains a vector of statements and provides iteration over them
  - Used to group related configuration elements

- **Section**:
  - Represents a named section in the configuration with a specific type
  - Section types are defined in the SectionType enum (DEVICE, INTERFACES, IP, etc.)
  - Contains a Block of statements for the section contents

- **Configuration**:
  - Top-level class that represents the entire network configuration
  - Contains a collection of Section objects
  - Serves as the entry point for the parsed configuration

### Advantages of This Class Design

This class-based approach offers several benefits:

1. **Hierarchical Representation**: Mirrors the natural structure of network configurations
2. **Type Safety**: Each configuration element has a specific type with appropriate operations
3. **Memory Management**: Structured cleanup through virtual destroy() methods prevents memory leaks
4. **Validation Support**: Enables type-specific validation of configuration values
5. **Serialization**: Easy conversion to string representation via to_string() methods
6. **Traversal**: Simple traversal of the configuration tree for analysis or transformation

### Integration with the Parser

The Bison parser (`parser.bison`) constructs this object hierarchy as it recognizes grammar patterns:

1. When tokens are recognized, appropriate objects (Value, Property, etc.) are instantiated
2. These objects are combined into more complex structures according to grammar rules
3. Sections are populated with properties and nested sections
4. The final result is a complete Configuration object that represents the parsed network configuration
5. The parser stores this result in the `parser_result` variable for further processing

### Configuration Evaluation and Representation

Each class implements:

- **destroy()**: Properly cleans up resources used by the configuration structure
- **to_string()**: Converts the configuration element to a human-readable string form
- **get_** methods: Provide access to the internal data of each configuration element

This design pattern allows for efficient parsing while maintaining a clean separation between the parser's grammar rules and the semantic representation of parsed network configurations. The resulting object hierarchy can be easily traversed, analyzed, and transformed for various purposes, such as validation, optimization, or generation of device-specific commands.

## Conclusion

The parser is designed to validate and process Mikrotik configurations written in our DSL. It enforces a clean, readable syntax without unnecessary punctuation like semicolons, focusing on a hierarchical structure that mirrors the logical organization of network configurations.

By following the syntax rules described in this document, you can create valid configurations that will be accepted by the parser and eventually translated into Mikrotik-compatible commands. 