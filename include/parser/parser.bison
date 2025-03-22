%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern char* yytext;
extern int line_number;
extern FILE* yyin;
int yyerror(const char* s);

/* For section tracking */
typedef enum {
    SECTION_NONE,
    SECTION_DEVICE,
    SECTION_INTERFACES,
    SECTION_IP,
    SECTION_ROUTING,
    SECTION_FIREWALL,
    SECTION_SYSTEM
} SectionType;

SectionType current_section = SECTION_NONE;
int nesting_level = 0;

void enter_section(SectionType section, const char* name) {
    current_section = section;
    nesting_level = 1;
    printf("Debug: Entering section %d (%s)\n", section, name);
}

void enter_block(const char* name) {
    nesting_level++;
    printf("Debug: Increasing nesting to %d, entering block %s\n", nesting_level, name);
}

void exit_block() {
    nesting_level--;
    printf("Debug: Decreasing nesting to %d\n", nesting_level);
    
    if (nesting_level == 0) {
        current_section = SECTION_NONE;
        printf("Debug: Leaving section\n");
    }
}
%}

%define parse.error verbose

/* Enable location tracking for better error messages */
%locations

/* Enable debugging features */
/* %define parse.trace */ /* Commented out to avoid redefinition warning */

/* Define value types for tokens */
%union {
    const char* str_val;
    int int_val;
}

/* Tokens from flex scanner */
%token TOKEN_COLON TOKEN_EQUALS TOKEN_LEFT_BRACKET TOKEN_RIGHT_BRACKET
%token TOKEN_LEFT_BRACE TOKEN_RIGHT_BRACE TOKEN_COMMA TOKEN_SLASH
%token TOKEN_MINUS TOKEN_DOT
%token TOKEN_SEMICOLON  /* Add semicolon token to explicitly handle it */

/* Keyword tokens */
%token TOKEN_DEVICE TOKEN_VENDOR TOKEN_INTERFACES TOKEN_IP TOKEN_ROUTING
%token TOKEN_FIREWALL TOKEN_SYSTEM TOKEN_TYPE TOKEN_ADMIN_STATE
%token TOKEN_DESCRIPTION TOKEN_ETHERNET TOKEN_SPEED TOKEN_DUPLEX
%token TOKEN_VLAN TOKEN_VLAN_ID TOKEN_INTERFACE TOKEN_ADDRESS
%token TOKEN_DHCP TOKEN_DHCP_CLIENT TOKEN_DHCP_SERVER
%token TOKEN_STATIC_ROUTE_DEFAULT_GW TOKEN_DESTINATION TOKEN_GATEWAY
%token TOKEN_CHAIN TOKEN_CONNECTION_STATE TOKEN_ACTION
%token TOKEN_INPUT TOKEN_OUTPUT TOKEN_FORWARD TOKEN_SRCNAT
%token TOKEN_MASQUERADE TOKEN_ENABLED TOKEN_DISABLED
%token TOKEN_ACCEPT TOKEN_DROP TOKEN_REJECT

/* Literal tokens */
%token <str_val> TOKEN_IDENTIFIER TOKEN_STRING TOKEN_BOOL
%token <int_val> TOKEN_NUMBER
%token <str_val> TOKEN_IP_ADDRESS TOKEN_IP_CIDR TOKEN_IP_RANGE
%token <str_val> TOKEN_IPV6_ADDRESS TOKEN_IPV6_CIDR TOKEN_IPV6_RANGE

/* UNKNOWN */
%token TOKEN_UNKNOWN

/* Non-terminals */
%type <str_val> any_identifier property_name section_name

/* Define precedence to help resolve shift/reduce conflicts */
%left TOKEN_COLON
%left TOKEN_EQUALS
%left TOKEN_COMMA
%left TOKEN_LEFT_BRACKET TOKEN_RIGHT_BRACKET

/* Start symbol */
%start config

%%

config
    : section_list
    ;

section_list
    : section
    | section_list section
    ;

section
    : section_header block {
        exit_block();  /* When a section is complete, reset section context */
    }
    ;

section_header
    : section_name TOKEN_COLON {
        if (strcmp($1, "device") == 0) enter_section(SECTION_DEVICE, "device");
        else if (strcmp($1, "interfaces") == 0) enter_section(SECTION_INTERFACES, "interfaces");
        else if (strcmp($1, "ip") == 0) enter_section(SECTION_IP, "ip");
        else if (strcmp($1, "routing") == 0) enter_section(SECTION_ROUTING, "routing");
        else if (strcmp($1, "firewall") == 0) enter_section(SECTION_FIREWALL, "firewall");
        else if (strcmp($1, "system") == 0) enter_section(SECTION_SYSTEM, "system");
    }
    ;

section_name
    : TOKEN_DEVICE { $$ = "device"; }
    | TOKEN_INTERFACES { $$ = "interfaces"; }
    | TOKEN_IP { $$ = "ip"; }
    | TOKEN_ROUTING { $$ = "routing"; }
    | TOKEN_FIREWALL { $$ = "firewall"; }
    | TOKEN_SYSTEM { $$ = "system"; }
    ;

block
    : /* empty */
    | TOKEN_LEFT_BRACE statement_list TOKEN_RIGHT_BRACE
    | statement_list
    ;

statement_list
    : statement
    | statement_list statement
    ;

statement
    : property_statement
    | subsection_statement
    | error_statement  /* Add a rule to catch syntax errors */
    ;

error_statement
    : TOKEN_SEMICOLON {
        yyerror("Semicolons are not allowed in this DSL");
        YYERROR;  /* Force error handling */
    }
    | TOKEN_UNKNOWN {
        yyerror("Unknown token or invalid syntax encountered");
        YYERROR;
    }
    | property_name error {
        yyerror("Invalid property assignment syntax");
        YYERROR;
    }
    | value error {
        yyerror("Unexpected token after value");
        YYERROR;
    }
    | error TOKEN_EQUALS {
        yyerror("Invalid token before equals sign");
        YYERROR;
    }
    | error TOKEN_COLON {
        yyerror("Invalid token before colon");
        YYERROR;
    }
    ;

property_statement
    : property_name TOKEN_EQUALS value
    ;

subsection_statement
    : any_identifier TOKEN_COLON {
        enter_block($1);  /* Entering a nested block */
    } block {
        exit_block();   /* Exiting a nested block */
    }
    | any_identifier TOKEN_COLON TOKEN_SEMICOLON {
        /* Explicitly catch the case where a semicolon follows a colon */
        yyerror("Invalid syntax: semicolon after colon. After a section declaration, only a newline or comment is allowed");
        YYERROR;
    }
    | any_identifier TOKEN_COLON error {
        /* Catch any other token after a colon that's not followed by valid block content */
        yyerror("Invalid syntax after colon. After a section declaration, only a newline or comment is allowed");
        YYERROR;
    }
    ;

/* Generic property name to cover all token types that can appear before equals */
property_name
    : TOKEN_IDENTIFIER { $$ = (const char*)strdup(yytext); }
    | TOKEN_VENDOR { $$ = "vendor"; }
    | TOKEN_TYPE { $$ = "type"; }
    | TOKEN_ADMIN_STATE { $$ = "admin_state"; }
    | TOKEN_DESCRIPTION { $$ = "description"; }
    | TOKEN_ADDRESS { $$ = "address"; }
    | TOKEN_STATIC_ROUTE_DEFAULT_GW { $$ = "static_route_default_gw"; }
    | TOKEN_CHAIN { $$ = "chain"; }
    | TOKEN_CONNECTION_STATE { $$ = "connection_state"; }
    | TOKEN_ACTION { $$ = "action"; }
    | TOKEN_SPEED { $$ = "speed"; }
    | TOKEN_DUPLEX { $$ = "duplex"; }
    | TOKEN_VLAN_ID { $$ = "vlan_id"; }
    | TOKEN_INTERFACE { $$ = "interface"; }
    | TOKEN_DESTINATION { $$ = "destination"; }
    | TOKEN_GATEWAY { $$ = "gateway"; }
    ;

/* Generic identifier to cover all token types that can appear before colon */
any_identifier
    : TOKEN_IDENTIFIER { $$ = (const char*)strdup(yytext); }
    | TOKEN_ETHERNET { $$ = "ethernet"; }
    | TOKEN_VLAN { $$ = "vlan"; }
    | TOKEN_IP { $$ = "ip"; }
    | TOKEN_DHCP { $$ = "dhcp"; }
    | TOKEN_DHCP_SERVER { $$ = "dhcp_server"; }
    | TOKEN_DHCP_CLIENT { $$ = "dhcp_client"; }
    ;

value
    : simple_value
    | list_value
    ;

simple_value
    : TOKEN_STRING
    | TOKEN_NUMBER
    | TOKEN_BOOL
    | TOKEN_IP_ADDRESS
    | TOKEN_IP_CIDR
    | TOKEN_IP_RANGE
    | TOKEN_IPV6_ADDRESS
    | TOKEN_IPV6_CIDR
    | TOKEN_IPV6_RANGE
    | TOKEN_ENABLED
    | TOKEN_DISABLED
    | TOKEN_INPUT
    | TOKEN_OUTPUT
    | TOKEN_FORWARD
    | TOKEN_SRCNAT
    | TOKEN_ACCEPT
    | TOKEN_DROP
    | TOKEN_REJECT
    | TOKEN_MASQUERADE
    ;

list_value
    : TOKEN_LEFT_BRACKET value_list TOKEN_RIGHT_BRACKET
    ;

value_list
    : simple_value
    | value_list TOKEN_COMMA simple_value
    ;

%%

int yyerror(const char* s) {
    fprintf(stderr, "Parse error at line %d: %s\n", line_number, s);
    return 1;
} 