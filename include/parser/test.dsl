# Configuración Completa de Router Mikrotik
# Incluye IPv4, IPv6, VLANs, Firewall, NAT y más

device:
    vendor = "mikrotik"
    model = "CCR2004-1G-12S+2XS"
    serial = "HG723KL991"

system:
    hostname = "core-router-01"
    timezone = "America/Caracas"
    ntp:
        enabled = true
        primary_server = "time.google.com"
        backup_server = "pool.ntp.org"
    logging:
        remote_server = "192.168.1.250"
        topics = ["error", "warning", "info"]

interfaces:
    # WAN1 - Conexión Principal Internet
    ether1:
        type = "ethernet"
        admin_state = "enabled"
        description = "WAN1 - ISP Principal"
        ethernet:
            speed = "auto"
            duplex = "full"
            mtu = 1500
        ipv4:
            dhcp_client = true
            default_route_distance = 1
        ipv6:
            address = 2001:db8:1::1/64
            default_route_distance = 1

    # WAN2 - Conexión Backup
    ether2:
        type = "ethernet"
        admin_state = "enabled"
        description = "WAN2 - ISP Backup"
        ethernet:
            speed = "1g"
            duplex = "full"
        ipv4:
            address = 200.200.200.2/30
            default_route_distance = 10
        ipv6:
            address = 2001:db8:2::2/64
            default_route_distance = 10

    # LAN Principal
    ether3:
        type = "ethernet"
        admin_state = "enabled"
        description = "LAN Core Switch"
        ethernet:
            speed = "10g"
            duplex = "full"
        ipv4:
            address = 192.168.1.1/24
        ipv6:
            address = fd00:1:1::1/64

    # VLAN Corporativa
    vlan100:
        type = "vlan"
        admin_state = "enabled"
        interface = ether3
        vlan_id = 100
        description = "VLAN Corporativa"
        ipv4:
            address = 192.168.10.1/24
        ipv6:
            address = fd00:10::1/64

    # VLAN Invitados
    vlan200:
        type = "vlan"
        admin_state = "enabled"
        interface = ether3
        vlan_id = 200
        description = "VLAN Invitados"
        ipv4:
            address = 192.168.20.1/24
        ipv6:
            address = fd00:20::1/64

ip:
    dhcp_server:
        lan_principal:
            interface = ether3
            address_pool = 192.168.1.100-192.168.1.200
            lease_time = 12h
            dns_servers = ["8.8.8.8", "8.8.4.4"]
            gateway = 192.168.1.1

        vlan_corporativa:
            interface = vlan100
            address_pool = 192.168.10.100-192.168.10.200
            lease_time = 24h
            dns_servers = ["192.168.1.1", "8.8.8.8"]
            gateway = 192.168.10.1

        vlan_invitados:
            interface = vlan200
            address_pool = 192.168.20.100-192.168.20.200
            lease_time = 2h
            dns_servers = ["8.8.8.8"]
            gateway = 192.168.20.1

    dhcpv6_server:
        lan_principal:
            interface = ether3
            address_pool = fd00:1:1::100-fd00:1:1::1ff
            lease_time = 12h
            dns_servers = ["2001:4860:4860::8888"]

routing:
    bgp:
        enabled = true
        as_number = 65000
        router_id = "192.168.1.1"
        peers = [
            {
                name = "ISP1",
                remote_as = 64512,
                address = 200.200.200.1
            },
            {
                name = "ISP2",
                remote_as = 64513,
                address = "2001:db8:2::1"
            }
        ]

    ospf:
        enabled = true
        router_id = "192.168.1.1"
        areas = [
            {
                area_id = "0.0.0.0",
                networks = ["192.168.0.0/16"]
            }
        ]

firewall:
    filter:
        # Reglas generales de entrada
        input_accept_established:
            chain = input
            connection_state = ["established", "related"]
            action = accept

        input_drop_invalid:
            chain = input
            connection_state = invalid
            action = drop

        # Reglas para servicios específicos
        input_accept_ssh_admin:
            chain = input
            protocol = tcp
            dst_port = 22
            src_address = 192.168.1.0/24
            action = accept

        # Reglas de forward entre VLANs
        forward_vlan_corporate_to_internet:
            chain = forward
            src_address = 192.168.10.0/24
            out_interface = [ether1, ether2]
            action = accept

        forward_vlan_guest_restricted:
            chain = forward
            src_address = 192.168.20.0/24
            dst_port = [80, 443]
            protocol = tcp
            action = accept

    nat:
        srcnat_wan1:
            chain = srcnat
            out_interface = ether1
            action = masquerade

        srcnat_wan2:
            chain = srcnat
            out_interface = ether2
            action = masquerade

        dstnat_public_web:
            chain = dstnat
            protocol = tcp
            dst_port = 80
            in_interface = ether1
            dst_address = 200.200.200.2
            to_addresses = 192.168.1.100
            to_ports = 80

# Configuración de QoS
qos:
    interface_ether1:
        download_rate = "100M"
        upload_rate = "20M"
        priority_queues = [
            {
                name = "voip",
                limit_at = "10M",
                max_limit = "20M",
                priority = 1,
                dst_port = [5060, 5061]
            },
            {
                name = "web",
                limit_at = "5M",
                max_limit = "10M",
                priority = 8,
                dst_port = [80, 443]
            }
        ]