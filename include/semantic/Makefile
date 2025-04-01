# Semantic Analysis Module Makefile

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -I../../include

# Directory settings
SRCDIR = ./
ASTDIR = ../ast
BINDIR = ../../bin/semantic
OBJDIR = ../../obj/semantic

# Source files
SEMANTIC_SRCS = $(SRCDIR)/semantic_table.cpp
DEMO_SRCS = $(SRCDIR)/demo.cpp
# Exclude example.cpp which has its own main function
AST_SRCS = $(filter-out $(ASTDIR)/example.cpp, $(wildcard $(ASTDIR)/*.cpp))

# Object files
SEMANTIC_OBJS = $(SEMANTIC_SRCS:%.cpp=$(OBJDIR)/%.o)
DEMO_OBJS = $(DEMO_SRCS:%.cpp=$(OBJDIR)/%.o)
AST_OBJS = $(AST_SRCS:%.cpp=$(OBJDIR)/%.o)

# Target binaries
DEMO_TARGET = $(BINDIR)/semantic_demo

# Create directories
$(shell mkdir -p $(BINDIR))
$(shell mkdir -p $(OBJDIR))
$(shell mkdir -p $(OBJDIR)/$(SRCDIR))
$(shell mkdir -p $(OBJDIR)/$(ASTDIR))

# Default target
all: $(DEMO_TARGET)

# Demo binary
$(DEMO_TARGET): $(DEMO_OBJS) $(SEMANTIC_OBJS) $(AST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Run the demo program
run: all
	@echo "=== Running Semantic Analysis Demo ==="
	@$(DEMO_TARGET)
	@echo "=== Demo completed ==="

# Pattern rule for object files
$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean target
clean:
	rm -rf $(BINDIR)/* $(OBJDIR)/*

# Rebuild target
rebuild: clean all

# Phony targets
.PHONY: all clean rebuild run

# Dependencies
$(OBJDIR)/$(SRCDIR)/semantic_table.o: $(SRCDIR)/semantic_table.cpp $(SRCDIR)/semantic_table.hpp
$(OBJDIR)/$(SRCDIR)/demo.o: $(SRCDIR)/demo.cpp $(SRCDIR)/semantic_table.hpp

# Include AST dependencies
-include $(AST_OBJS:.o=.d) 