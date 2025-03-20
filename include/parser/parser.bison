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

/* Non-terminals */
%type <str_val> string_value identifier ip_address
%type <int_val> number

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
    : device_section
    | interfaces_section
    | ip_section
    | routing_section
    | firewall_section
    | system_section
    ;

device_section
    : TOKEN_DEVICE TOKEN_COLON device_properties
    ;

device_properties
    : device_property
    | device_properties device_property
    ;

device_property
    : TOKEN_VENDOR TOKEN_EQUALS string_value
    | TOKEN_IDENTIFIER TOKEN_EQUALS value
    ;

interfaces_section
    : TOKEN_INTERFACES TOKEN_COLON interface_list
    ;

interface_list
    : interface_def
    | interface_list interface_def
    ;

interface_def
    : identifier TOKEN_COLON interface_properties
    ;

interface_properties
    : interface_property
    | interface_properties interface_property
    ;

interface_property
    : TOKEN_TYPE TOKEN_EQUALS string_value
    | TOKEN_ADMIN_STATE TOKEN_EQUALS string_value
    | TOKEN_DESCRIPTION TOKEN_EQUALS string_value
    | TOKEN_ETHERNET TOKEN_COLON ethernet_properties
    | TOKEN_VLAN TOKEN_COLON vlan_properties
    | TOKEN_IP TOKEN_COLON ip_interface_properties
    | TOKEN_IDENTIFIER TOKEN_EQUALS value
    ;

ethernet_properties
    : ethernet_property
    | ethernet_properties ethernet_property
    ;

ethernet_property
    : TOKEN_SPEED TOKEN_EQUALS string_value
    | TOKEN_DUPLEX TOKEN_EQUALS string_value
    | TOKEN_IDENTIFIER TOKEN_EQUALS value
    ;

vlan_properties
    : vlan_property
    | vlan_properties vlan_property
    ;

vlan_property
    : TOKEN_VLAN_ID TOKEN_EQUALS value
    | TOKEN_INTERFACE TOKEN_EQUALS string_value
    | TOKEN_IDENTIFIER TOKEN_EQUALS value
    ;

ip_section
    : TOKEN_IP TOKEN_COLON ip_properties
    ;

ip_properties
    : ip_property
    | ip_properties ip_property
    ;

ip_property
    : TOKEN_DHCP TOKEN_COLON dhcp_properties
    | TOKEN_IDENTIFIER TOKEN_EQUALS value
    | identifier TOKEN_COLON ip_subproperties
    ;

dhcp_properties
    : dhcp_property
    | dhcp_properties dhcp_property
    ;

dhcp_property
    : TOKEN_DHCP_SERVER TOKEN_COLON dhcp_server_list
    | TOKEN_DHCP_CLIENT TOKEN_COLON dhcp_client_properties
    | TOKEN_IDENTIFIER TOKEN_EQUALS value
    ;

dhcp_server_list
    : dhcp_server_def
    | dhcp_server_list dhcp_server_def
    ;

dhcp_server_def
    : identifier TOKEN_COLON dhcp_server_properties
    ;

dhcp_server_properties
    : dhcp_server_property
    | dhcp_server_properties dhcp_server_property
    ;

dhcp_server_property
    : TOKEN_IDENTIFIER TOKEN_EQUALS value
    ;

dhcp_client_properties
    : dhcp_client_property
    | dhcp_client_properties dhcp_client_property
    ;

dhcp_client_property
    : TOKEN_IDENTIFIER TOKEN_EQUALS value
    ;

ip_interface_properties
    : ip_interface_property
    | ip_interface_properties ip_interface_property
    ;

ip_interface_property
    : TOKEN_ADDRESS TOKEN_EQUALS value
    | TOKEN_IDENTIFIER TOKEN_EQUALS value
    ;

ip_subproperties
    : ip_subproperty
    | ip_subproperties ip_subproperty
    ;

ip_subproperty
    : TOKEN_IDENTIFIER TOKEN_EQUALS value
    ;

routing_section
    : TOKEN_ROUTING TOKEN_COLON routing_properties
    ;

routing_properties
    : routing_property
    | routing_properties routing_property
    ;

routing_property
    : TOKEN_STATIC_ROUTE_DEFAULT_GW TOKEN_EQUALS value
    | identifier TOKEN_COLON route_properties
    ;

route_properties
    : route_property
    | route_properties route_property
    ;

route_property
    : TOKEN_DESTINATION TOKEN_EQUALS value
    | TOKEN_GATEWAY TOKEN_EQUALS value
    | TOKEN_IDENTIFIER TOKEN_EQUALS value
    ;

firewall_section
    : TOKEN_FIREWALL TOKEN_COLON firewall_properties
    ;

firewall_properties
    : firewall_property
    | firewall_properties firewall_property
    ;

firewall_property
    : identifier TOKEN_COLON firewall_chain_properties
    ;

firewall_chain_properties
    : firewall_rule_def
    | firewall_chain_properties firewall_rule_def
    ;

firewall_rule_def
    : identifier TOKEN_COLON firewall_rule_properties
    ;

firewall_rule_properties
    : firewall_rule_property
    | firewall_rule_properties firewall_rule_property
    ;

firewall_rule_property
    : TOKEN_CHAIN TOKEN_EQUALS chain_value
    | TOKEN_CONNECTION_STATE TOKEN_EQUALS list_value
    | TOKEN_ACTION TOKEN_EQUALS action_value
    | TOKEN_IDENTIFIER TOKEN_EQUALS value
    ;

chain_value
    : TOKEN_INPUT
    | TOKEN_OUTPUT
    | TOKEN_FORWARD
    | TOKEN_SRCNAT
    | TOKEN_IDENTIFIER
    ;

action_value
    : TOKEN_ACCEPT
    | TOKEN_DROP
    | TOKEN_REJECT
    | TOKEN_MASQUERADE
    | TOKEN_IDENTIFIER
    ;

system_section
    : TOKEN_SYSTEM TOKEN_COLON system_properties
    ;

system_properties
    : system_property
    | system_properties system_property
    ;

system_property
    : TOKEN_IDENTIFIER TOKEN_EQUALS value
    ;

value
    : string_value
    | number
    | TOKEN_BOOL
    | ip_address
    | list_value
    ;

list_value
    : TOKEN_LEFT_BRACKET value_list TOKEN_RIGHT_BRACKET
    ;

value_list
    : value
    | value_list TOKEN_COMMA value
    ;

string_value
    : TOKEN_STRING
    ;

number
    : TOKEN_NUMBER
    ;

identifier
    : TOKEN_IDENTIFIER
    ;

ip_address
    : TOKEN_IP_ADDRESS
    | TOKEN_IP_CIDR
    | TOKEN_IP_RANGE
    | TOKEN_IPV6_ADDRESS
    | TOKEN_IPV6_CIDR
    | TOKEN_IPV6_RANGE
    ;

%%

int yyerror(const char* s) {
    fprintf(stderr, "Parse error at line %d: %s\n", line_number, s);
    return 1;
} 