# Abstract Syntax Tree (AST) for Mikrotik DSL

This directory contains the AST implementation for our Mikrotik network configuration DSL. The AST represents the hierarchical structure of a configuration after it has been parsed.

## Core Components

### 1. AST Node Interface (`ast_node_interface.hpp/cpp`)

Base interface that all AST nodes inherit from. Defines common methods for:
- Memory management (`destroy()`)
- String representation (`to_string()`)

### 2. Data Types (`datatype.hpp/cpp`)

Represents the types of values in our DSL:
- Basic types (string, number, boolean)
- Network-specific types (IP addresses, CIDR notation)
- Container types (lists)

### 3. Expressions (`expression.hpp/cpp`)

Represents values and expressions in the DSL:
- Literal values (strings, numbers, booleans, IP addresses)
- Collections (lists)
- References (identifiers, property access)

### 4. Statements (`statement.hpp/cpp`)

Represents actions and groupings in the configuration:
- Property assignments (`vendor = "mikrotik"`)
- Blocks (collections of statements)
- Sections (named blocks like `device:`, `interfaces:`)

### 5. Declarations (`declaration.hpp/cpp`)

Represents named entities in the configuration:
- Configuration sections
- Properties
- Interfaces
- The program itself (top-level container)

## Memory Management

All AST nodes manage their memory through the `destroy()` method, which:
- Cleans up dynamically allocated memory
- Recursively destroys child nodes
- Sets pointers to null to prevent double-free issues

## Example

The repository includes an example of how to use the AST:

- `example.dsl`: Sample Mikrotik DSL configuration
- `example.cpp`: C++ code that builds the corresponding AST
  
To build and run the example:

```
# Build the example
make

# Run the example
make run
```

Alternatively, you can run the executable directly:

```
./bin/ast/example
```

This will:
1. Create an AST that represents the configuration in `example.dsl`
2. Print the AST's string representation
3. Properly clean up the AST's memory

## Building

To build the AST library:

```
make
```

This will create:
- A static library `bin/ast/libast.a` that can be linked with other components of the compiler
- All object files in the `bin/ast` directory
- The example executable in `bin/ast/example`

To clean build artifacts:

```
make clean
```

## Usage

The AST is typically built by the parser as it recognizes language constructs. Once built, the AST can be:
1. Semantically analyzed to check for errors
2. Transformed for optimization
3. Used to generate code for the target platform (Mikrotik RouterOS) 