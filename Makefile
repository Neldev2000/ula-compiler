CC = gcc
CFLAGS = -Wall -I include/parser
BUILD_DIR = build
PARSER_DIR = include/parser

# Scanner files
SCANNER_SRC = $(PARSER_DIR)/scanner.c
SCANNER_LEX = $(PARSER_DIR)/flex.l
SCANNER_GEN = $(PARSER_DIR)/lex.yy.c
SCANNER_BIN = $(BUILD_DIR)/scanner

# Test files
TEST_FILE = $(PARSER_DIR)/test.dsl

# Targets
.PHONY: all clean test dirs

all: $(SCANNER_BIN)

dirs:
	mkdir -p $(BUILD_DIR)

# Generate scanner C code from flex
$(SCANNER_GEN): $(SCANNER_LEX)
	flex -o $@ $<

# Build scanner binary
$(SCANNER_BIN): dirs $(SCANNER_GEN) $(SCANNER_SRC)
	$(CC) $(CFLAGS) -o $@ $(SCANNER_GEN) $(SCANNER_SRC)

# Run test
test: $(SCANNER_BIN)
	./$(SCANNER_BIN) $(TEST_FILE)

# Clean generated files
clean:
	rm -f $(SCANNER_GEN)
	rm -rf $(BUILD_DIR)
