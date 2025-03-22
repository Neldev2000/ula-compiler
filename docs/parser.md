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

## Conclusion

The parser is designed to validate and process Mikrotik configurations written in our DSL. It enforces a clean, readable syntax without unnecessary punctuation like semicolons, focusing on a hierarchical structure that mirrors the logical organization of network configurations.

By following the syntax rules described in this document, you can create valid configurations that will be accepted by the parser and eventually translated into Mikrotik-compatible commands. 