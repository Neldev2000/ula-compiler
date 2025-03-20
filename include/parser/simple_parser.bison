%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern char* yytext;
extern int line_number;
extern FILE* yyin;
int yyerror(const char* s);
%}

%define parse.error verbose

/* Define value types for tokens */
%union {
    char* str_val;
    int int_val;
}

/* Tokens from flex scanner */
%token TOKEN_COLON TOKEN_EQUALS TOKEN_LEFT_BRACKET TOKEN_RIGHT_BRACKET
%token TOKEN_LEFT_BRACE TOKEN_RIGHT_BRACE TOKEN_COMMA TOKEN_SLASH
%token TOKEN_MINUS TOKEN_DOT

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
    : TOKEN_DEVICE TOKEN_COLON block
    | TOKEN_INTERFACES TOKEN_COLON block
    | TOKEN_IP TOKEN_COLON block
    | TOKEN_ROUTING TOKEN_COLON block
    | TOKEN_FIREWALL TOKEN_COLON block
    | TOKEN_SYSTEM TOKEN_COLON block
    ;

block
    : /* empty */
    | statement
    | block statement
    ;

statement
    : TOKEN_IDENTIFIER TOKEN_COLON block
    | TOKEN_IDENTIFIER TOKEN_EQUALS value
    | TOKEN_VENDOR TOKEN_EQUALS value
    | TOKEN_TYPE TOKEN_EQUALS value
    | TOKEN_ADMIN_STATE TOKEN_EQUALS value
    | TOKEN_DESCRIPTION TOKEN_EQUALS value
    | TOKEN_ADDRESS TOKEN_EQUALS value
    | TOKEN_STATIC_ROUTE_DEFAULT_GW TOKEN_EQUALS value
    | TOKEN_CHAIN TOKEN_EQUALS value
    | TOKEN_CONNECTION_STATE TOKEN_EQUALS value
    | TOKEN_ACTION TOKEN_EQUALS value
    | TOKEN_ETHERNET TOKEN_COLON block
    | TOKEN_VLAN TOKEN_COLON block
    | TOKEN_IP TOKEN_COLON block
    | TOKEN_DHCP TOKEN_COLON block
    | TOKEN_DHCP_SERVER TOKEN_COLON block
    | TOKEN_DHCP_CLIENT TOKEN_COLON block
    ;

value
    : TOKEN_STRING
    | TOKEN_NUMBER
    | TOKEN_BOOL
    | TOKEN_IP_ADDRESS
    | TOKEN_IP_CIDR
    | TOKEN_IP_RANGE
    | TOKEN_IPV6_ADDRESS
    | TOKEN_IPV6_CIDR
    | TOKEN_IPV6_RANGE
    | TOKEN_LEFT_BRACKET value_list TOKEN_RIGHT_BRACKET
    ;

value_list
    : value
    | value_list TOKEN_COMMA value
    ;

%%

int yyerror(const char* s) {
    fprintf(stderr, "Parse error at line %d: %s\n", line_number, s);
    return 1;
} 