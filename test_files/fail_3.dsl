# Simple FAIL test file (it shouldn´t have semi-colons, they are not in the DSL language) 
device:
    vendor = "mikrotik";
    model = "test";

interfaces:
    ether1:
        type = "ethernet";
        description = "Test interface";
        ip:
            address = 192.168.1.1/24; 