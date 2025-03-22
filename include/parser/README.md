# DSL Parser for Mikrotik Network Configuration

This directory contains the parser implementation for our Mikrotik Network Configuration DSL. The parser validates DSL files against the language grammar and reports syntax errors.

## Directory Structure

- `parser.bison` - The LALR(1) grammar definition for our DSL
- `scanner.flex` - The lexical analyzer (tokenizer) definition
- `parser.c` - The main program file for the parser
- `Makefile` - Build scripts to compile and test the parser

## Building the Parser

To build the parser, simply run:

```bash
make
```

This will create the `dsl_parser` executable in the `../../bin/parser/` directory.

## Running the Parser

You can run the parser on a single DSL file:

```bash
../../bin/parser/dsl_parser path/to/file.dsl
```

The parser will output whether the file passed or failed validation.

## Testing

### Testing Commands

The Makefile includes several testing commands:

1. **Test a Single File**:
   ```bash
   make test
   ```
   This runs the parser on a predefined test file (`../../test/parser/code.dsl`).

2. **Run a Simple Test**:
   ```bash
   make test_simple
   ```
   This tests the parser on a simplified DSL file (`../../test/parser/simpler.dsl`).

3. **Test Multiple Sections**:
   ```bash
   make test_multi
   ```
   This tests the parser on a file with multiple sections (`../../test/parser/test_multi.dsl`).

4. **Run All Tests**:
   ```bash
   make test_all
   ```
   This is the most comprehensive testing command that runs the parser on all test files.

### Understanding `make test_all`

The `test_all` command automatically:

1. Runs the parser on all DSL files in the `../../test_files/` directory
2. Verifies that regular files (without the `fail_` prefix) pass validation
3. Verifies that files with the `fail_` prefix fail validation as expected
4. Provides a summary of the test results

Example output:
```
Running all tests...
====================
Testing files expected to PASS:
--------------------
Testing code.dsl
PASS: code.dsl passed as expected
Testing sample.dsl
PASS: sample.dsl passed as expected
Testing simpler.dsl
PASS: simpler.dsl passed as expected
Testing test_multi.dsl
PASS: test_multi.dsl passed as expected
4/4 valid files passed

Testing files expected to FAIL:
--------------------
Testing fail_1.dsl
PASS: fail_1.dsl failed as expected
Testing fail_2.dsl
PASS: fail_2.dsl failed as expected
Testing fail_3.dsl
PASS: fail_3.dsl failed as expected
3/3 invalid files failed as expected
====================
```

### Test File Naming Convention

- Regular test files should be named `*.dsl` (e.g., `code.dsl`, `sample.dsl`)
- Files that are expected to fail validation should be prefixed with `fail_` (e.g., `fail_1.dsl`, `fail_2.dsl`)

### Creating New Test Files

To add new test cases:

1. For valid DSL files: Add your DSL file to the `../../test_files/` directory
2. For invalid DSL files: Add your file with the `fail_` prefix to the same directory
3. Run `make test_all` to verify that your new test cases are processed correctly

## Cleaning up

To clean all generated files:

```bash
make clean
```

This removes all object files, generated parser files, and the parser executable.

## Error Messages

When a DSL file fails validation, the parser provides descriptive error messages, including:
- The line number where the error occurred
- The nature of the syntax error
- Suggestions for how to fix the error

For example:
```
Parse error at line 3: Semicolons are not allowed in this DSL
``` 