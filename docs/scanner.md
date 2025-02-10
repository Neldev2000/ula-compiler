# Tokenization of the DSL Language for Mikrotik Network Configuration

## Introduction to Tokenization

**Tokenization**, also known as lexical analysis, is the first phase of the compilation process. The **scanner** (or tokenizer) takes the source code of our DSL as input and breaks it down into a sequence of meaningful lexical units called **tokens**. Each token represents a basic element of the language, such as keywords, identifiers, operators, literals, and punctuation symbols.

The goal of the tokenizer is to simplify the input for the next phase, the **parser** (syntactic analysis). Instead of working directly with the character stream of the source code, the parser works with the token sequence, which makes it easier to identify the grammatical structure of the DSL program.

For this project, we are specifically focusing on creating a DSL to configure **Mikrotik** devices. Therefore, while the tokenization process is general, the examples and context will be tailored to Mikrotik configuration.

## Token Types in Our Mikrotik DSL

Our DSL for Mikrotik configuration is composed of the following token types:

### 1. Punctuation Tokens
These tokens represent special characters used for syntax:
- `TOKEN_COLON` (:) - Used for block definitions
- `TOKEN_EQUALS` (=) - Used for assignments
- `TOKEN_LEFT_BRACKET` ([) - Start of a list
- `TOKEN_RIGHT_BRACKET` (]) - End of a list
- `TOKEN_LEFT_BRACE` ({) - Start of an object
- `TOKEN_RIGHT_BRACE` (}) - End of an object
- `TOKEN_COMMA` (,) - List separator
- `TOKEN_DOT` (.) - Used in IP addresses and hierarchical names
- `TOKEN_MINUS` (-) - Used in ranges
- `TOKEN_SLASH` (/) - Used in CIDR notation

### 2. Keywords
Reserved identifiers that have a special meaning in the DSL:

#### Device Configuration
- `TOKEN_DEVICE` - Device section
- `TOKEN_VENDOR` - Vendor specification
- `TOKEN_SYSTEM` - System configuration section

#### Network Interfaces
- `TOKEN_INTERFACES` - Interface configuration section
- `TOKEN_TYPE` - Interface type specification
- `TOKEN_ADMIN_STATE` - Administrative state (enabled/disabled)
- `TOKEN_DESCRIPTION` - Interface description
- `TOKEN_ETHERNET` - Ethernet interface settings
- `TOKEN_SPEED` - Interface speed setting
- `TOKEN_DUPLEX` - Interface duplex mode
- `TOKEN_VLAN` - VLAN interface
- `TOKEN_VLAN_ID` - VLAN identifier
- `TOKEN_INTERFACE` - Interface reference

#### IP Configuration
- `TOKEN_IP` - IP configuration section
- `TOKEN_ADDRESS` - IP address assignment
- `TOKEN_DHCP` - DHCP configuration
- `TOKEN_DHCP_CLIENT` - DHCP client settings
- `TOKEN_DHCP_SERVER` - DHCP server settings

#### Routing
- `TOKEN_ROUTING` - Routing configuration section
- `TOKEN_STATIC_ROUTE_DEFAULT_GW` - Default gateway
- `TOKEN_DESTINATION` - Route destination
- `TOKEN_GATEWAY` - Gateway address

#### Firewall
- `TOKEN_FIREWALL` - Firewall configuration section
- `TOKEN_CHAIN` - Firewall chain
- `TOKEN_CONNECTION_STATE` - Connection state matching
- `TOKEN_ACTION` - Firewall action
- `TOKEN_INPUT` - Input chain
- `TOKEN_OUTPUT` - Output chain
- `TOKEN_FORWARD` - Forward chain
- `TOKEN_SRCNAT` - Source NAT
- `TOKEN_MASQUERADE` - Masquerade NAT
- `TOKEN_ACCEPT` - Accept action
- `TOKEN_DROP` - Drop action
- `TOKEN_REJECT` - Reject action

#### States
- `TOKEN_ENABLED` - Enabled state
- `TOKEN_DISABLED` - Disabled state

### 3. Literals and Identifiers

#### Basic Types
- `TOKEN_IDENTIFIER` - Names and identifiers (e.g., interface names)
- `TOKEN_NUMBER` - Numeric values
- `TOKEN_STRING` - Text in double quotes
- `TOKEN_BOOL` - Boolean values (true/false)

#### Network Address Types
- `TOKEN_IP_ADDRESS` - IPv4 addresses (e.g., 192.168.1.1)
- `TOKEN_IP_CIDR` - IPv4 CIDR notation (e.g., 192.168.1.0/24)
- `TOKEN_IP_RANGE` - IPv4 address range (e.g., 192.168.1.1-192.168.1.254)
- `TOKEN_IPV6_ADDRESS` - IPv6 addresses (e.g., 2001:db8::1)
- `TOKEN_IPV6_CIDR` - IPv6 CIDR notation (e.g., 2001:db8::/64)
- `TOKEN_IPV6_RANGE` - IPv6 address range (e.g., 2001:db8::1-2001:db8::ff)

### 4. Comments
The scanner supports two types of comments:
- Single-line comments starting with `#`
- Multi-line comments enclosed in triple quotes `"""`

## Examples

### Basic Device Configuration
```dsl
device:
    vendor = "mikrotik"
    model = "CCR2004-1G-12S+2XS"
```

### Interface Configuration with IPv6
```dsl
interfaces:
    ether1:
        type = "ethernet"
        admin_state = "enabled"
        ipv6:
            address = 2001:db8::1/64
```

### Firewall Rule with Object
```dsl
firewall:
    filter:
        input_accept_established:
            chain = input
            connection_state = ["established", "related"]
            action = accept
```

### Multi-line Comments
```dsl
"""
This is a multi-line comment
that can span multiple lines
and is useful for documentation
"""