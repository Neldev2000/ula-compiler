# Tokenización del Lenguaje DSL para Configuración de Red

## Introducción a la Tokenización

La **tokenización**, también conocida como análisis léxico, es la primera fase del proceso de compilación. El **scanner** (o tokenizer) toma el código fuente de nuestro DSL como entrada y lo divide en una secuencia de unidades léxicas significativas llamadas **tokens**. Cada token representa un elemento básico del lenguaje, como palabras clave, identificadores, operadores, literales y símbolos de puntuación.

El objetivo del tokenizer es simplificar la entrada para la siguiente fase, el **parser** (análisis sintáctico).  En lugar de trabajar directamente con el flujo de caracteres del código fuente, el parser trabaja con la secuencia de tokens, lo que facilita la identificación de la estructura gramatical del programa DSL.

## Tipos de Tokens en Nuestro DSL

Nuestro DSL se compone de los siguientes tipos de tokens:

### 1. Palabras Clave (Keywords)

Las **palabras clave** son identificadores reservados que tienen un significado especial en el lenguaje DSL.  No pueden ser utilizados como identificadores definidos por el usuario. En nuestro DSL, las palabras clave son:

*   `device`
*   `vendor`
*   `interfaces`
*   `ip`
*   `routing`
*   `firewall`
*   `olt`
*   `switch`
*   `fortinet_firewall`
*   `vpn`
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
*   `static_route_default_gw`
*   `destination`
*   `gateway`
*   `chain`
*   `connection_state`
*   `action`
*   `pon_ports`
*   `pon1`
*   `profile`
*   `onu_management`
*   `auto_onu_provisioning`
*   `vlans`
*   `interface_configuration`
*   `port_mode`
*   `access_vlan`
*   `policies`
*   `policy_allow_web`
*   `source_interface`
*   `destination_interface`
*   `source_address`
*   `destination_address`
*   `service`
*   `ipsec_tunnel_site1`
*   `local_address`
*   `remote_address`
*   `authentication_method`
*   `preshared_key`
*   `hostname`
*   `timezone`
*   `ntp_server`
*   `enabled`
*   `disabled`
*   `accept`
*   `drop`
*   `reject`
*   `masquerade`

**Ejemplo de Palabras Clave en el DSL:**

```dsl
device:
  vendor = "mikrotik"

interfaces:
  ether1:
    type = "ethernet"
    admin_state = "enabled"
    dhcp_client = true