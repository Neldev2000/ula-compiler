# This is a sample Mikrotik configuration in our DSL language

device:
    vendor = "mikrotik"
    model = "CCR2004-1G-12S+2XS"
    hostname = "core-router"

# Interface configuration
interfaces:
    ether1:
        type = "ethernet"
        admin_state = "enabled"
        description = "WAN Connection"
        ethernet:
            speed = "auto"
            duplex = "auto"
        ip:
            address = 192.168.1.1/24
            
    ether2:
        type = "ethernet"
        admin_state = "enabled"
        description = "LAN Connection"
        ip:
            address = 10.0.0.1/24
            
    vlan100:
        type = "vlan"
        admin_state = "enabled"
        description = "Management VLAN"
        vlan:
            vlan_id = 100
            interface = "ether2"
        ip:
            address = 10.100.0.1/24

# IP configuration
ip:
    dhcp:
        dhcp_server:
            lan:
                network = 10.0.0.0/24
                gateway = 10.0.0.1
                dns = "8.8.8.8"
                lease_time = "1d"
                address_pool = 10.0.0.10-10.0.0.254

# Routing configuration
routing:
    static_route_default_gw = 192.168.1.254
    static_route1:
        destination = 172.16.0.0/24
        gateway = 10.0.0.254

# Firewall configuration
firewall:
    filter:
        input_accept_established:
            chain = input
            connection_state = ["established", "related"]
            action = accept
            
        input_drop_all:
            chain = input
            action = drop
            
    nat:
        srcnat_masquerade:
            chain = srcnat
            action = masquerade
            out_interface = "ether1"

# System configuration
system:
    identity = "MikroTik Core Router"
    ntp_client = "enabled"
    ntp_server = "pool.ntp.org" 