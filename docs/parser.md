# Parsing the DSL Language for Mikrotik Network Configuration

## Introduction to Parsing

**Parsing**, also known as syntactic analysis, is the second phase of the compilation process. The **parser** takes the token sequence produced by the scanner and analyzes it to determine the grammatical structure according to the language grammar. The parser ensures that the token sequence follows the syntax rules defined for the DSL.

During parsing, the parser constructs a representation of the input, typically in the form of a parse tree or abstract syntax tree (AST), which captures the hierarchical structure of the input program. This tree representation is then used in subsequent phases of compilation, such as semantic analysis and code generation.

The goal of the parser is to validate that the input program follows the language's grammar rules and to organize the tokens into a structured representation that captures the program's meaning.

For our Mikrotik network configuration DSL, the parser needs to handle hierarchical configurations, nested blocks, and various types of property assignments, while maintaining context awareness across different sections.

## LALR(1) Parsing for Our Mikrotik DSL

Our parser uses a **LALR(1)** (Look-Ahead LR) parsing technique, which offers a good balance between power and efficiency. This approach was chosen after evaluating several options:

| Grammar Type | Pros | Cons | Assessment |
|--------------|------|------|------------|
| LL(1) | Simple to understand and implement | Cannot handle left recursion; requires grammar transformations | Not suitable for our hierarchical structure |
| LR(0) | Simple implementation; efficient parsing | Too restrictive; no lookahead capability | Too limited for our language needs |
| LR(1) | Most powerful deterministic parser | Very large parse tables; memory intensive | Overkill for our relatively straightforward DSL |
| LALR(1) | Good balance between power and efficiency; smaller tables than LR(1) | Slightly less powerful than LR(1) | **Selected** - Best balance for our hierarchical configuration DSL |

### Key Features of Our Parser

1. **Context-Sensitive Parsing**: The parser tracks the current section and nesting level to properly handle transitions between sections and nested blocks.

2. **Flattened Grammar**: Instead of having specific rules for every possible configuration element, we use a more generic approach with a flattened grammar structure.

3. **Explicit Section Boundaries**: Clear delineation of where sections begin and end helps with handling transitions between major configuration components.

4. **Generic Rule Structure**: We use generic rules for properties and blocks, which reduces grammar complexity and shift/reduce conflicts.

## Grammar Structure

Our DSL grammar for Mikrotik configuration is organized hierarchically:

### 1. Top-Level Structure

```
config → section_list

section_list → section
              | section_list section

section → section_header block
```

### 2. Section Types

The DSL supports six main section types:
- `device`: Basic device information
- `interfaces`: Network interface configuration
- `ip`: IP addressing and services
- `routing`: Routing protocols and static routes
- `firewall`: Firewall rules and NAT
- `system`: System-level configuration

### 3. Block Structure

Blocks can contain property assignments or nested blocks:

```
block → ε (empty)
       | statement_list

statement_list → statement
                | statement_list statement

statement → property_statement
           | block_statement
```

### 4. Property and Block Statements

```
property_statement → property_name "=" value

block_statement → any_identifier ":" block
```

### 5. Values

The grammar supports various value types:

```
value → string
       | number
       | boolean
       | ip_address
       | list_value
       | ...
```

## Parsing Techniques and Challenges

### 1. Section Context Tracking

To properly handle section transitions, our parser maintains context information:

```c
typedef enum {
    SECTION_NONE,
    SECTION_DEVICE,
    SECTION_INTERFACES,
    SECTION_IP,
    SECTION_ROUTING,
    SECTION_FIREWALL,
    SECTION_SYSTEM
} SectionType;

SectionType current_section = SECTION_NONE;
int nesting_level = 0;
```

This context tracking helps the parser recognize when it's exiting one section and entering another, which was one of the main challenges in earlier implementations.

### 2. Block Nesting

Handling nested blocks requires careful tracking of nesting levels:

```c
void enter_block(const char* name) {
    nesting_level++;
    // Process block entry
}

void exit_block() {
    nesting_level--;
    
    if (nesting_level == 0) {
        current_section = SECTION_NONE;
        // We've exited the main section
    }
}
```

### 3. Generic Rules vs. Specific Rules

Our implementation balances generic and specific rules:

- For common structures (like property assignments), we use generic rules
- For section-specific elements, we use more targeted rules

This approach reduces grammar complexity while still enforcing structural validity.

## Examples of DSL Parsing

### Example 1: Basic Device Configuration

```dsl
device:
    vendor = "mikrotik"
    model = "CCR2004-1G-12S+2XS"
```

**Parsing Process**:
1. Recognize `device:` as a section header
2. Enter the `device` section context
3. Parse `vendor = "mikrotik"` as a property assignment
4. Parse `model = "CCR2004-1G-12S+2XS"` as a property assignment
5. Exit the `device` section when no more tokens are available in this block

### Example 2: Nested Blocks

```dsl
interfaces:
    ether1:
        type = "ethernet"
        ethernet:
            speed = "1Gbps"
            duplex = "full"
```

**Parsing Process**:
1. Recognize `interfaces:` as a section header
2. Enter the `interfaces` section context
3. Recognize `ether1:` as a block statement and increase nesting level
4. Parse `type = "ethernet"` as a property assignment
5. Recognize `ethernet:` as a nested block and increase nesting level again
6. Parse `speed = "1Gbps"` and `duplex = "full"` as property assignments
7. Exit the `ethernet` block when no more tokens are available in this level
8. Exit the `ether1` block when no more tokens are available in this level
9. Exit the `interfaces` section when no more tokens are available in this block

### Example 3: Multiple Interface Definitions

```dsl
interfaces:
    ether1:
        type = "ethernet"
        admin_state = "enabled"
    ether2:
        type = "ethernet"
        admin_state = "disabled"
```

**Parsing Process**:
1. Recognize `interfaces:` as a section header
2. Enter the `interfaces` section context
3. Process `ether1:` block completely
4. Return to the `interfaces` section context
5. Process `ether2:` block completely
6. Exit the `interfaces` section when no more tokens are available

### Example 4: Section Transitions

```dsl
device:
    vendor = "mikrotik"
    
interfaces:
    ether1:
        type = "ethernet"
```

**Parsing Process**:
1. Process `device:` section completely
2. Reset section context to SECTION_NONE
3. Recognize `interfaces:` as a new section header
4. Enter the `interfaces` section context
5. Process the `interfaces` section content
6. Exit the `interfaces` section when no more tokens are available

## Handling Ambiguity

The DSL grammar contains some inherent ambiguities that require special handling:

### 1. Precedence Declarations

To resolve shift/reduce conflicts, we use precedence declarations:

```
%left TOKEN_COLON
%left TOKEN_EQUALS
```

These declarations help the parser decide between shifting (continuing to read) and reducing (applying a grammar rule) when faced with ambiguous input.

### 2. Context-Sensitive Decisions

Some parsing decisions depend on the current context. For example, the token `ip` could be:
- A section header (`ip:`)
- A property name (`ip = ...`)
- A nested block within another section (`interfaces: ether1: ip: ...`)

Our context tracking helps disambiguate these cases.

## Conclusion

The parser is a critical component in our Mikrotik configuration DSL compiler. It processes the tokens from the scanner, validates the syntax according to our grammar rules, and prepares the input for semantic analysis and code generation.

Our LALR(1) parser implementation with context tracking and a flattened grammar structure successfully handles the complexities of the Mikrotik configuration language, including nested blocks, multiple definitions at the same level, and transitions between major configuration sections.

This parser provides a solid foundation for further development of our DSL compiler, enabling network administrators to define Mikrotik configurations in a clean, hierarchical, and human-readable format. 