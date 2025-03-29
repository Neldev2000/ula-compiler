#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expressions.hpp"

extern FILE* yyin;
extern int yyparse();
extern int line_number;
extern int yydebug;
extern Configuration* parser_result;

void usage(char* argv[]) {
    printf("Usage: %s input_file\n", argv[0]);
    exit(1);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        usage(argv);
    }

    yyin = fopen(argv[1], "r");

    if (!yyin) {
        printf("Could not open %s\n", argv[1]);
        exit(1);
    }

    printf("Parsing %s...\n", argv[1]);
    
    /* Enable parser debugging */
    // yydebug = 1;
    
    int parse_result = yyparse();

    if (parse_result == 0) {
        printf("Parse successful! The input conforms to the Mikrotik DSL grammar.\n");
        
        // Print the parsed configuration using the class hierarchy
        if (parser_result) {
            printf("Parsed Configuration:\n%s\n", parser_result->to_string().c_str());
            
            // Clean up resources
            parser_result->destroy();
            delete parser_result;
        }
    } else {
        printf("Parse failed! The input contains syntax errors.\n");
    }

    fclose(yyin);
    
    return parse_result;
} 