CXX = g++
FLEX = flex
BISON = bison --defines=token.h

all: dsl_parser

dsl_parser: parser.o scanner.o main.o
	$(CXX) scanner.o parser.o main.o -o dsl_parser

parser.o: parser.c
	$(CXX) -c parser.c -DYYDEBUG=1

parser.c: enhanced_parser.bison
	$(BISON) -v -t --output parser.c enhanced_parser.bison

scanner.o: token.h scanner.c
	$(CXX) -c scanner.c

scanner.c: scanner.flex
	$(FLEX) -d -o scanner.c scanner.flex

main.o: token.h main.c
	$(CXX) -c main.c

test: dsl_parser
	./dsl_parser code.dsl

test_simple: dsl_parser
	./dsl_parser simpler.dsl

test_multi: dsl_parser
	./dsl_parser test_multi.dsl

.PHONY: clean
clean:
	$(RM) *.o parser.c parser.output token.h scanner.c dsl_parser 