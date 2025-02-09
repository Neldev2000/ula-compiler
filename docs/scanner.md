# Tokenization of the DSL Language for Mikrotik Network Configuration

## Introduction to Tokenization

**Tokenization**, also known as lexical analysis, is the first phase of the compilation process. The **scanner** (or tokenizer) takes the source code of our DSL as input and breaks it down into a sequence of meaningful lexical units called **tokens**. Each token represents a basic element of the language, such as keywords, identifiers, operators, literals, and punctuation symbols.

The goal of the tokenizer is to simplify the input for the next phase, the **parser** (syntactic analysis). Instead of working directly with the character stream of the source code, the parser works with the token sequence, which makes it easier to identify the grammatical structure of the DSL program.

For this project, we are specifically focusing on creating a DSL to configure **Mikrotik** devices. Therefore, while the tokenization process is general, the examples and context will be tailored to Mikrotik configuration.

## Token Types in Our Mikrotik DSL

Our DSL for Mikrotik configuration is composed of the following token types:

### 1. Keywords

**Keywords** are reserved identifiers that have a special meaning in the DSL language. They cannot be used as user-defined identifiers. In our DSL, the keywords related to Mikrotik configuration are:

*   `device`
*   `vendor`
*   `interfaces`
*   `ip`
*   `routing`
*   `firewall`
*   `system`
*   `type`
*   `admin_state`
*   `description`
*   `ethernet`
*   `speed`
*   `duplex`
*   `vlan`
*   `vlan_id`
*   `interface`
*   `address`
*   `dhcp`
*   `dhcp_client`
*   `dhcp_server`
*   `static_route_default_gw`
*   `destination`
*   `gateway`
*   `chain`
*   `connection_state`
*   `action`
*   `input`
*   `output`
*   `forward`
*   `srcnat`
*   `masquerade`
*   `enabled`
*   `disabled`
*   `accept`
*   `drop`
*   `reject`

**Example of Keywords in the Mikrotik DSL:**

```dsl
device:
  vendor = "mikrotik"

interfaces:
  ether1:
    type = "ethernet"
    admin_state = "enabled"
    dhcp_client = true