CC = gcc
CFLAGS = -Wall -Wextra
FLEX = flex
BISON = bison

# Files and directories
BUILD_DIR = build
PARSER_DIR = include/parser
SCANNER_DIR = include/scanner

all: $(BUILD_DIR)/parser

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Generate parser files (parser.tab.c and parser.tab.h)
$(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/parser.tab.h: $(PARSER_DIR)/parser.bison | $(BUILD_DIR)
	$(BISON) -d -o $(BUILD_DIR)/parser.tab.c $<

# Generate scanner
$(BUILD_DIR)/lex.yy.c: $(SCANNER_DIR)/flex.l $(BUILD_DIR)/parser.tab.h | $(BUILD_DIR)
	$(FLEX) -o $@ $<

# Compile scanner
$(BUILD_DIR)/lex.yy.o: $(BUILD_DIR)/lex.yy.c
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -Iinclude -c -o $@ $<

# Compile parser
$(BUILD_DIR)/parser.tab.o: $(BUILD_DIR)/parser.tab.c
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -Iinclude -c -o $@ $<

# Compile parser implementation
$(BUILD_DIR)/parser.o: $(PARSER_DIR)/parser.c $(BUILD_DIR)/parser.tab.h
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -Iinclude -c -o $@ $<

# Link everything
$(BUILD_DIR)/parser: $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/parser.tab.o $(BUILD_DIR)/parser.o
	$(CC) -o $@ $^

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
