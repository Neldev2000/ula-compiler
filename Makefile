CC = g++
CFLAGS = -Wall -std=c++17 -fpermissive -I.

FLEX = flex
BISON = bison

# Directory structure
SRC_DIR = src
BUILD_DIR = bin

# Output binary (ahora en la raíz)
OUTPUT = mikrotik_compiler

# Flex and Bison generated files
PARSER_C = $(BUILD_DIR)/parser.tab.c
PARSER_H = $(BUILD_DIR)/parser.tab.h
LEXER_C = $(BUILD_DIR)/lex.yy.c

# Source files (all cpp files in src directory)
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC))

# Object files
OBJECTS = $(BUILD_DIR)/parser.tab.o $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/parser.o $(OBJ)

all: $(OUTPUT)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Generate parser files with Bison (generate the .h file first)
$(PARSER_H): $(SRC_DIR)/parser.bison | $(BUILD_DIR)
	$(BISON) -d -o $(PARSER_C) $<

# Generate parser files with Bison
$(PARSER_C): $(PARSER_H)
	@echo "Parser C already generated with header"

# Generate lexer files with Flex - depends on the header being generated first
$(LEXER_C): $(SRC_DIR)/scanner.flex $(PARSER_H) | $(BUILD_DIR)
	$(FLEX) -o $@ $<

# Compile C++ files from src directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(PARSER_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# Compile parser.c (main.c)
$(BUILD_DIR)/parser.o: $(SRC_DIR)/main.c $(PARSER_H) $(LEXER_C) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# Compile parser.tab.c
$(BUILD_DIR)/parser.tab.o: $(PARSER_C) $(PARSER_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# Compile lex.yy.c
$(BUILD_DIR)/lex.yy.o: $(LEXER_C) $(PARSER_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -I$(SRC_DIR) -c $< -o $@

# Link all object files
$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(OUTPUT)

.PHONY: all clean 