#include <stdio.h>
#include <stdlib.h>
#include "../include/parser/parser.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    
    printf("Parsing file: %s\n", filename);
    
    int result = parse_file(filename);
    
    if (result) {
        printf("Parsing successful!\n");
    } else {
        printf("Parsing failed.\n");
    }
    
    return !result;  // Return 0 for success, 1 for failure
} 