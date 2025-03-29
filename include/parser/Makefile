SHELL = /bin/bash
CXX = g++
FLEX = flex
BISON = bison
BIN_DIR = ../../bin/parser
CXXFLAGS = -I$(BIN_DIR) -I. -std=c++11

all: $(BIN_DIR)/dsl_parser

$(BIN_DIR)/dsl_parser: $(BIN_DIR)/parser.o $(BIN_DIR)/scanner.o $(BIN_DIR)/parser.c.o $(BIN_DIR)/expressions.o
	$(CXX) $(BIN_DIR)/scanner.o $(BIN_DIR)/parser.o $(BIN_DIR)/parser.c.o $(BIN_DIR)/expressions.o -o $(BIN_DIR)/dsl_parser

# Copy expressions.hpp
$(BIN_DIR)/expressions.hpp: expressions.hpp
	mkdir -p $(BIN_DIR)
	cp expressions.hpp $(BIN_DIR)/

# Generate parser.c and parser.tab.h
$(BIN_DIR)/parser.c $(BIN_DIR)/parser.tab.h: parser.bison $(BIN_DIR)/expressions.hpp
	$(BISON) -v -t --defines=$(BIN_DIR)/parser.tab.h --output=$(BIN_DIR)/parser.c parser.bison

# Compile parser
$(BIN_DIR)/parser.o: $(BIN_DIR)/parser.c $(BIN_DIR)/expressions.hpp
	$(CXX) $(CXXFLAGS) -c $(BIN_DIR)/parser.c -DYYDEBUG=1 -o $(BIN_DIR)/parser.o

# Generate scanner
$(BIN_DIR)/scanner.c: scanner.flex $(BIN_DIR)/parser.tab.h $(BIN_DIR)/expressions.hpp
	$(FLEX) -d -o $(BIN_DIR)/scanner.c scanner.flex

# Compile scanner
$(BIN_DIR)/scanner.o: $(BIN_DIR)/scanner.c $(BIN_DIR)/parser.tab.h
	$(CXX) $(CXXFLAGS) -c $(BIN_DIR)/scanner.c -o $(BIN_DIR)/scanner.o

# Compile main parser.c
$(BIN_DIR)/parser.c.o: parser.c $(BIN_DIR)/expressions.hpp
	$(CXX) $(CXXFLAGS) -c parser.c -o $(BIN_DIR)/parser.c.o

# Compile expressions
$(BIN_DIR)/expressions.o: expressions.cpp $(BIN_DIR)/expressions.hpp
	cp expressions.cpp $(BIN_DIR)/
	$(CXX) $(CXXFLAGS) -c $(BIN_DIR)/expressions.cpp -o $(BIN_DIR)/expressions.o

test: $(BIN_DIR)/dsl_parser
	$(BIN_DIR)/dsl_parser ../../test/parser/code.dsl

test_simple: $(BIN_DIR)/dsl_parser
	$(BIN_DIR)/dsl_parser ../../test/parser/simpler.dsl

test_multi: $(BIN_DIR)/dsl_parser
	$(BIN_DIR)/dsl_parser ../../test/parser/test_multi.dsl

test_all: $(BIN_DIR)/dsl_parser
	@echo "Running all tests..."
	@echo "===================="
	@echo "Testing files expected to PASS:"
	@echo "--------------------"
	@pass=0; total=0; \
	for file in ../../test_files/*.dsl; do \
		if [[ $$(basename $$file) != fail_* ]]; then \
			echo "Testing $$(basename $$file)"; \
			$(BIN_DIR)/dsl_parser $$file > /dev/null 2>&1; \
			if [ $$? -eq 0 ]; then \
				echo "PASS: $$(basename $$file) passed as expected"; \
				pass=$$((pass+1)); \
			else \
				echo "FAIL: $$(basename $$file) failed but should pass"; \
			fi; \
			total=$$((total+1)); \
		fi; \
	done; \
	echo "$$pass/$$total valid files passed"
	@echo ""
	@echo "Testing files expected to FAIL:"
	@echo "--------------------"
	@pass=0; total=0; \
	for file in ../../test_files/fail_*.dsl; do \
		echo "Testing $$(basename $$file)"; \
		$(BIN_DIR)/dsl_parser $$file > /dev/null 2>&1; \
		if [ $$? -ne 0 ]; then \
			echo "PASS: $$(basename $$file) failed as expected"; \
			pass=$$((pass+1)); \
		else \
			echo "FAIL: $$(basename $$file) passed but should fail"; \
		fi; \
		total=$$((total+1)); \
	done; \
	echo "$$pass/$$total invalid files failed as expected"
	@echo ""
	@echo "===================="

.PHONY: clean test test_simple test_multi test_all
clean:
	$(RM) $(BIN_DIR)/*.o $(BIN_DIR)/parser.c $(BIN_DIR)/parser.output $(BIN_DIR)/parser.tab.h $(BIN_DIR)/token.h $(BIN_DIR)/scanner.c $(BIN_DIR)/dsl_parser 