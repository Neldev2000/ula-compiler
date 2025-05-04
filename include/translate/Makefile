# MikroTik Translation Module Makefile

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -I../../include

# Directory settings
SRCDIR = ./
ASTDIR = ../ast
SEMDIR = ../semantic
BINDIR = ../../bin/translate
OBJDIR = ../../obj/translate

# Source files
TRANSLATE_SRCS = $(SRCDIR)/demo.cpp
# Exclude example.cpp from AST sources to avoid multiple main functions
AST_SRCS = $(filter-out $(ASTDIR)/example.cpp, $(wildcard $(ASTDIR)/*.cpp))
SEMANTIC_SRCS = $(SEMDIR)/semantic_table.cpp

# Object files
TRANSLATE_OBJS = $(TRANSLATE_SRCS:%.cpp=$(OBJDIR)/%.o)
AST_OBJS = $(AST_SRCS:%.cpp=$(OBJDIR)/%.o)
SEMANTIC_OBJS = $(SEMANTIC_SRCS:%.cpp=$(OBJDIR)/%.o)

# Target binaries
DEMO_TARGET = $(BINDIR)/demo

# Create directories
$(shell mkdir -p $(BINDIR))
$(shell mkdir -p $(OBJDIR))
$(shell mkdir -p $(OBJDIR)/$(SRCDIR))
$(shell mkdir -p $(OBJDIR)/$(ASTDIR))
$(shell mkdir -p $(OBJDIR)/$(SEMDIR))

# Default target
all: $(DEMO_TARGET)

# Demo binary
$(DEMO_TARGET): $(TRANSLATE_OBJS) $(AST_OBJS) $(SEMANTIC_OBJS)
	@echo "Building MikroTik translator demo program..."
	@$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Build complete: $(DEMO_TARGET)"

# Run the demo program
run: all
	@echo "=== Running MikroTik Translation Demo ==="
	@$(DEMO_TARGET)
	@echo "=== Demo completed ==="
	@echo "Output written to demo_program.txt"

# Pattern rule for object files
$(OBJDIR)/%.o: %.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean target
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BINDIR)/* $(OBJDIR)/*
	@echo "Clean complete"

# Rebuild target
rebuild: clean all

# Phony targets
.PHONY: all clean rebuild run

# Dependencies
$(OBJDIR)/$(SRCDIR)/demo.o: $(SRCDIR)/demo.cpp $(ASTDIR)/*.hpp $(SEMDIR)/*.hpp

# Include AST and Semantic dependencies
-include $(AST_OBJS:.o=.d)
-include $(SEMANTIC_OBJS:.o=.d)

# Print information about the build
info:
	@echo "MikroTik Translation Module Build Information"
	@echo "----------------------------------------"
	@echo "Source files:"
	@echo "  TRANSLATE: $(TRANSLATE_SRCS)"
	@echo "  AST: $(AST_SRCS)"
	@echo "  SEMANTIC: $(SEMANTIC_SRCS)"
	@echo "Object files will be placed in: $(OBJDIR)"
	@echo "Binary files will be placed in: $(BINDIR)"
	@echo "----------------------------------------"
	@echo "Available targets:"
	@echo "  all      - Build the translator demo (default)"
	@echo "  run      - Build and run the translator demo"
	@echo "  clean    - Remove all built files"
	@echo "  rebuild  - Clean and rebuild"
	@echo "  info     - Display this information"

.PHONY: info 