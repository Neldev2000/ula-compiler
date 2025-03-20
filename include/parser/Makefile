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

$(BIN_DIR)/main.o: $(BIN_DIR)/token.h main.c
	$(CXX) -c main.c -o $(BIN_DIR)/main.o

test: $(BIN_DIR)/dsl_parser
	$(BIN_DIR)/dsl_parser ../../test/parser/code.dsl

test_simple: $(BIN_DIR)/dsl_parser
	$(BIN_DIR)/dsl_parser ../../test/parser/simpler.dsl

test_multi: $(BIN_DIR)/dsl_parser
	$(BIN_DIR)/dsl_parser ../../test/parser/test_multi.dsl

.PHONY: clean
clean:
	$(RM) $(BIN_DIR)/*.o $(BIN_DIR)/parser.c $(BIN_DIR)/parser.output $(BIN_DIR)/token.h $(BIN_DIR)/scanner.c $(BIN_DIR)/dsl_parser 