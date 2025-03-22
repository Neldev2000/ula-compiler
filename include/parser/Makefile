SHELL = /bin/bash
CXX = g++
FLEX = flex
BISON = bison --defines=$(BIN_DIR)/token.h
BIN_DIR = ../../bin/parser

all: $(BIN_DIR)/dsl_parser

$(BIN_DIR)/dsl_parser: $(BIN_DIR)/parser.o $(BIN_DIR)/scanner.o $(BIN_DIR)/main.o
	$(CXX) $(BIN_DIR)/scanner.o $(BIN_DIR)/parser.o $(BIN_DIR)/main.o -o $(BIN_DIR)/dsl_parser

$(BIN_DIR)/parser.o: $(BIN_DIR)/parser.c
	$(CXX) -c $(BIN_DIR)/parser.c -DYYDEBUG=1 -o $(BIN_DIR)/parser.o

$(BIN_DIR)/parser.c: parser.bison
	$(BISON) -v -t --output $(BIN_DIR)/parser.c parser.bison

$(BIN_DIR)/scanner.o: $(BIN_DIR)/token.h $(BIN_DIR)/scanner.c
	$(CXX) -c $(BIN_DIR)/scanner.c -o $(BIN_DIR)/scanner.o

$(BIN_DIR)/scanner.c: scanner.flex
	$(FLEX) -d -o $(BIN_DIR)/scanner.c scanner.flex

$(BIN_DIR)/main.o: $(BIN_DIR)/token.h parser.c
	$(CXX) -c parser.c -o $(BIN_DIR)/main.o

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
	$(RM) $(BIN_DIR)/*.o $(BIN_DIR)/parser.c $(BIN_DIR)/parser.output $(BIN_DIR)/token.h $(BIN_DIR)/scanner.c $(BIN_DIR)/dsl_parser 