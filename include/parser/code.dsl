# This is a sample Mikrotik configuration in our DSL language

device:
    vendor = "mikrotik"
    model = "CCR2004-1G-12S+2XS"
    hostname = "core-router"

# Simplified interface configuration
interfaces:
    ether1:
        type = "ethernet"
        admin_state = "enabled"
        description = "WAN Connection"

# IP section 
ip:
    dhcp:
        dhcp_server:
            lan:
                network = 10.0.0.0/24
                gateway = 10.0.0.1
                address_pool = 10.0.0.10-10.0.0.254

# Simplified routing configuration
routing:
    static_route_default_gw = 192.168.1.254

# Simplified firewall configuration
firewall:
    filter:
        input_accept_established:
            chain = input
            action = accept

# System configuration
system:
    identity = "MikroTik Core Router"
    ntp_client = "enabled"
    ntp_server = "pool.ntp.org" 