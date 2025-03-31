/*
    Mikrotik DSL: Abstract Syntax Tree (AST)

    This program creates the AST for the following Mikrotik DSL configuration:

    device:
        vendor = "mikrotik"
        model = "CCR2004-1G-12S+2XS"
    
    interfaces:
        ether1:
            type = "ethernet"
            admin_state = "enabled"
            ip:
                address = 192.168.1.1/24
        ether2:
            type = "ethernet"
            admin_state = "enabled"
            description = "WAN Connection"
    
    firewall:
        filter:
            input_accept_established:
                chain = "input"
                connection_state = ["established", "related"]
                action = "accept"
*/

#include <iostream>
#include "ast_node_interface.hpp"
#include "datatype.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "declaration.hpp"

int main() 
{
    // Create the program declaration with default constructor
    auto program = new ProgramDeclaration();
    
    // ---- DEVICE SECTION ----
    auto device_section = new SectionStatement{
        "device", 
        SectionStatement::SectionType::DEVICE,
        new BlockStatement{
            {
                new PropertyStatement{"vendor", new StringValue{"mikrotik"}},
                new PropertyStatement{"model", new StringValue{"CCR2004-1G-12S+2XS"}}
            }
        }
    };
    program->add_section(device_section);
    
    // ---- INTERFACES SECTION ----
    auto interfaces_section = new SectionStatement{
        "interfaces",
        SectionStatement::SectionType::INTERFACES,
        new BlockStatement{
            {
                // ether1 interface
                new SectionStatement{
                    "ether1",
                    SectionStatement::SectionType::CUSTOM,
                    new BlockStatement{
                        {
                            new PropertyStatement{"type", new StringValue{"ethernet"}},
                            new PropertyStatement{"admin_state", new StringValue{"enabled"}},
                            // IP subsection
                            new SectionStatement{
                                "ip",
                                SectionStatement::SectionType::IP,
                                new BlockStatement{
                                    {
                                        new PropertyStatement{"address", new IPCIDRValue{"192.168.1.1/24"}}
                                    }
                                }
                            }
                        }
                    }
                },
                
                // ether2 interface
                new SectionStatement{
                    "ether2",
                    SectionStatement::SectionType::CUSTOM,
                    new BlockStatement{
                        {
                            new PropertyStatement{"type", new StringValue{"ethernet"}},
                            new PropertyStatement{"admin_state", new StringValue{"enabled"}},
                            new PropertyStatement{"description", new StringValue{"WAN Connection"}}
                        }
                    }
                }
            }
        }
    };
    program->add_section(interfaces_section);
    
    // ---- FIREWALL SECTION ----
    auto firewall_section = new SectionStatement{
        "firewall",
        SectionStatement::SectionType::FIREWALL,
        new BlockStatement{
            {
                new SectionStatement{
                    "filter",
                    SectionStatement::SectionType::CUSTOM,
                    new BlockStatement{
                        {
                            new SectionStatement{
                                "input_accept_established",
                                SectionStatement::SectionType::CUSTOM,
                                new BlockStatement{
                                    {
                                        new PropertyStatement{"chain", new StringValue{"input"}},
                                        new PropertyStatement{
                                            "connection_state", 
                                            new ListValue{
                                                {
                                                    new StringValue{"established"},
                                                    new StringValue{"related"}
                                                }
                                            }
                                        },
                                        new PropertyStatement{"action", new StringValue{"accept"}}
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    };
    program->add_section(firewall_section);
    
    // ---- PRINT THE AST ----
    std::cout << "=== Mikrotik DSL AST Example ===" << std::endl;
    std::cout << program->to_string() << std::endl;
    
    // ---- CLEANUP ----
    std::cout << "Cleaning up AST memory..." << std::endl;
    program->destroy();
    delete program;
    
    std::cout << "Done." << std::endl;
    
    return 0;
} 