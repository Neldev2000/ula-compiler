CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -O2 -I.

# Output directory for binaries and objects
BIN_DIR = ../../bin/ast

AST_SOURCES = ast_node_interface.cpp datatype.cpp expression.cpp statement.cpp declaration.cpp
AST_OBJECTS = $(patsubst %.cpp,$(BIN_DIR)/%.o,$(AST_SOURCES))

.PHONY: all clean example dir run

all: dir $(BIN_DIR)/libast.a $(BIN_DIR)/example

# Create the output directory
dir:
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/libast.a: $(AST_OBJECTS)
	ar rcs $@ $^

$(BIN_DIR)/example: example.cpp $(BIN_DIR)/libast.a
	$(CXX) $(CXXFLAGS) -o $@ $< $(BIN_DIR)/libast.a

$(BIN_DIR)/%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/ast_node_interface.o: ast_node_interface.cpp ast_node_interface.hpp

$(BIN_DIR)/datatype.o: datatype.cpp datatype.hpp ast_node_interface.hpp

$(BIN_DIR)/expression.o: expression.cpp expression.hpp datatype.hpp ast_node_interface.hpp

$(BIN_DIR)/statement.o: statement.cpp statement.hpp expression.hpp datatype.hpp ast_node_interface.hpp

$(BIN_DIR)/declaration.o: declaration.cpp declaration.hpp statement.hpp expression.hpp ast_node_interface.hpp

# Run the example
run: $(BIN_DIR)/example
	@echo "Running example program..."
	@$(BIN_DIR)/example

clean:
	rm -rf $(BIN_DIR) 