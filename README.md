# NetForge Compiler Project

A MikroTik script compiler for the NetForge framework.

## Project Overview

NetForge is a compiler for MikroTik RouterOS scripts, designed to provide enhanced syntax, error checking, and optimizations over standard RouterOS scripting.

## Prerequisites

- C++17 compatible compiler (g++)
- Flex (Fast Lexical Analyzer)
- Bison (Parser Generator)
- Make

## Directory Structure

```
compiler-project/
├── bin/              # Compiled binaries
├── src/              # Source code
│   ├── *.cpp         # C++ implementation files
│   ├── *.h           # Header files
│   ├── scanner.flex  # Flex lexical analyzer definition
│   ├── parser.bison  # Bison parser definition
│   ├── main.c        # Main compiler entry point
│   └── Makefile      # Build script
├── examples/         # Example MikroTik scripts
└── README.md         # This file
```

## Building the Compiler

To build the compiler, navigate to the `src` directory and run make:

```bash
cd src
make
```

This will:
1. Generate the lexer and parser code from flex/bison definitions
2. Compile all source files
3. Create the executable at `../bin/mikrotik_compiler`

## Running the Compiler

Once compiled, you can run the compiler with:

```bash
./bin/mikrotik_compiler [input_file] [options]
```

### Basic Usage

```bash
./bin/mikrotik_compiler input.script
```

### Example

```bash
./bin/mikrotik_compiler examples/hello_world.script
```

## Cleaning the Build

To clean the build artifacts:

```bash
cd src
make clean
```

## License

[Add your license information here]

## Contributors

[Add contributor information here]
