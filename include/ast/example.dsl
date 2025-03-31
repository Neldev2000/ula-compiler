# Mikrotik DSL Configuration Example

# Basic device information
device:
    vendor = "mikrotik"
    model = "CCR2004-1G-12S+2XS"

# Network interfaces configuration
interfaces:
    # LAN interface
    ether1:
        type = "ethernet"
        admin_state = "enabled"
        ip:
            address = 192.168.1.1/24
    
    # WAN interface
    ether2:
        type = "ethernet"
        admin_state = "enabled"
        description = "WAN Connection"

# Firewall configuration
firewall:
    filter:
        input_accept_established:
            chain = "input"
            connection_state = ["established", "related"]
            action = "accept" 