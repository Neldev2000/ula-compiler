#pragma once

typedef enum {
    TOKEN_EOF = 0,
    
    // Custom tokens start at 258
    // Punctuation tokens
    TOKEN_COLON = 258,
    TOKEN_EQUALS,
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,      // Para objetos {
    TOKEN_RIGHT_BRACE,     // Para objetos }
    TOKEN_LEFT_BRACKET,    // Para listas [
    TOKEN_RIGHT_BRACKET,   // Para listas ]
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_MINUS,
    TOKEN_SLASH,          // Para CIDR notation (/)
    TOKEN_MULTILINE_COMMENT, // Para comentarios multilínea """..."""

    // Keywords
    TOKEN_DEVICE,
    TOKEN_VENDOR,
    TOKEN_INTERFACES,
    TOKEN_IP,
    TOKEN_ROUTING,
    TOKEN_FIREWALL,
    TOKEN_SYSTEM,
    TOKEN_TYPE,
    TOKEN_ADMIN_STATE,
    TOKEN_DESCRIPTION,
    TOKEN_ETHERNET,
    TOKEN_SPEED,
    TOKEN_DUPLEX,
    TOKEN_VLAN,
    TOKEN_VLAN_ID,
    TOKEN_INTERFACE,
    TOKEN_ADDRESS,
    TOKEN_DHCP,
    TOKEN_DHCP_CLIENT,
    TOKEN_DHCP_SERVER,
    TOKEN_STATIC_ROUTE_DEFAULT_GW,
    TOKEN_DESTINATION,
    TOKEN_GATEWAY,
    TOKEN_CHAIN,
    TOKEN_CONNECTION_STATE,
    TOKEN_ACTION,
    TOKEN_INPUT,
    TOKEN_OUTPUT,
    TOKEN_FORWARD,
    TOKEN_SRCNAT,
    TOKEN_MASQUERADE,
    TOKEN_ENABLED,
    TOKEN_DISABLED,
    TOKEN_ACCEPT,
    TOKEN_DROP,
    TOKEN_REJECT,

    // Literals and identifiers
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_BOOL,
    TOKEN_IPV6_ADDRESS,   // Para direcciones IPv6
    TOKEN_IPV6_CIDR,      // Para direcciones IPv6 con CIDR
    TOKEN_IPV6_RANGE,     // Para rangos de IPv6
    TOKEN_IP_ADDRESS,     // Para direcciones IP
    TOKEN_IP_CIDR,        // Para direcciones IP con CIDR
    TOKEN_IP_RANGE        // Para rangos de IP
} token_t;

inline const char* to_str(token_t token) {
    switch (token) {
        case TOKEN_COLON: return "COLON";
        case TOKEN_EQUALS: return "EQUALS";
        case TOKEN_LEFT_PAREN: return "LEFT_PAREN";
        case TOKEN_RIGHT_PAREN: return "RIGHT_PAREN";
        case TOKEN_LEFT_BRACE: return "LEFT_BRACE";
        case TOKEN_RIGHT_BRACE: return "RIGHT_BRACE";
        case TOKEN_LEFT_BRACKET: return "LEFT_BRACKET";
        case TOKEN_RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_DOT: return "DOT";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_MULTILINE_COMMENT: return "MULTILINE_COMMENT";
        case TOKEN_DEVICE: return "DEVICE";
        case TOKEN_VENDOR: return "VENDOR";
        case TOKEN_INTERFACES: return "INTERFACES";
        case TOKEN_IP: return "IP";
        case TOKEN_ROUTING: return "ROUTING";
        case TOKEN_FIREWALL: return "FIREWALL";
        case TOKEN_SYSTEM: return "SYSTEM";
        case TOKEN_TYPE: return "TYPE";
        case TOKEN_ADMIN_STATE: return "ADMIN_STATE";
        case TOKEN_DESCRIPTION: return "DESCRIPTION";
        case TOKEN_ETHERNET: return "ETHERNET";
        case TOKEN_SPEED: return "SPEED";
        case TOKEN_DUPLEX: return "DUPLEX";
        case TOKEN_VLAN: return "VLAN";
        case TOKEN_VLAN_ID: return "VLAN_ID";
        case TOKEN_INTERFACE: return "INTERFACE";
        case TOKEN_ADDRESS: return "ADDRESS";
        case TOKEN_DHCP: return "DHCP";
        case TOKEN_DHCP_CLIENT: return "DHCP_CLIENT";
        case TOKEN_DHCP_SERVER: return "DHCP_SERVER";
        case TOKEN_STATIC_ROUTE_DEFAULT_GW: return "STATIC_ROUTE_DEFAULT_GW";
        case TOKEN_DESTINATION: return "DESTINATION";
        case TOKEN_GATEWAY: return "GATEWAY";
        case TOKEN_CHAIN: return "CHAIN";
        case TOKEN_CONNECTION_STATE: return "CONNECTION_STATE";
        case TOKEN_ACTION: return "ACTION";
        case TOKEN_INPUT: return "INPUT";
        case TOKEN_OUTPUT: return "OUTPUT";
        case TOKEN_FORWARD: return "FORWARD";
        case TOKEN_SRCNAT: return "SRCNAT";
        case TOKEN_MASQUERADE: return "MASQUERADE";
        case TOKEN_ENABLED: return "ENABLED";
        case TOKEN_DISABLED: return "DISABLED";
        case TOKEN_ACCEPT: return "ACCEPT";
        case TOKEN_DROP: return "DROP";
        case TOKEN_REJECT: return "REJECT";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_BOOL: return "BOOL";
        case TOKEN_IPV6_ADDRESS: return "IPV6_ADDRESS";
        case TOKEN_IPV6_CIDR: return "IPV6_CIDR";
        case TOKEN_IPV6_RANGE: return "IPV6_RANGE";
        case TOKEN_IP_ADDRESS: return "IP_ADDRESS";
        case TOKEN_IP_CIDR: return "IP_CIDR";
        case TOKEN_IP_RANGE: return "IP_RANGE";
        default: return "UNKNOWN";
    }
}
