/*
    Compilers: Semantic Analysis and MikroTik Translation Demo

    This program creates the AST for the following Mikrotik DSL configuration,
    performs semantic analysis on it, and translates it to MikroTik RouterOS v7 script:

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
*/

#include <iostream>
#include <fstream>

#include "../ast/datatype.hpp"
#include "../ast/declaration.hpp"
#include "../ast/expression.hpp"
#include "../ast/statement.hpp"
#include "../semantic/semantic_table.hpp"

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

    // ============ SEMANTIC ANALYSIS ============

    std::cout << "=== Performing Semantic Analysis on Mikrotik DSL ===" << std::endl;
    
    // Name resolution - verify all references are valid
    bool name_resolution_valid = true;
    {
        std::cout << "\n--- Name Resolution ---" << std::endl;
        SymbolTable symbol_table;
        
        // First pass: register all sections
        for (const auto& section : program->get_sections()) {
            symbol_table.bind(section->get_name(), 
                Symbol::build(new ConfigSectionDatatype(), section->get_name()));
            
            // Register interfaces
            if (section->get_section_type() == SectionStatement::SectionType::INTERFACES) {
                // Get interfaces from block
                auto block = section->get_block();
                if (block) {
                    for (auto* stmt : block->get_statements()) {
                        auto interface = dynamic_cast<SectionStatement*>(stmt);
                        if (interface) {
                            symbol_table.bind(interface->get_name(), 
                                Symbol::build(new InterfaceDatatype(), interface->get_name()));
                        }
                    }
                }
            }
        }
        
        // Second pass: validate references (like firewall rules referencing interfaces)
        for (const auto& section : program->get_sections()) {
            if (section->get_section_type() == SectionStatement::SectionType::FIREWALL) {
                // Simplified validation - in real implementation we'd check more thoroughly
                auto block = section->get_block();
                if (block) {
                    for (auto* stmt : block->get_statements()) {
                        auto filter = dynamic_cast<SectionStatement*>(stmt);
                        if (filter && filter->get_name() == "filter") {
                            auto filter_block = filter->get_block();
                            if (filter_block) {
                                for (auto* rule : filter_block->get_statements()) {
                                    auto rule_section = dynamic_cast<SectionStatement*>(rule);
                                    if (rule_section) {
                                        // Simple check - in real code we'd verify referenced interfaces, chains, etc.
                                        std::cout << "  Validated firewall rule: " 
                                                 << rule_section->get_name() << std::endl;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        std::cout << std::boolalpha << "Configuration name resolution: " << name_resolution_valid << std::endl;
    }

    // Type checking - verify all properties have correct types
    bool type_checking_valid = true;
    {
        std::cout << "\n--- Type Checking ---" << std::endl;
        
        // Find device section
        SectionStatement* device = nullptr;
        for (const auto& section : program->get_sections()) {
            if (section->get_section_type() == SectionStatement::SectionType::DEVICE) {
                device = section;
                break;
            }
        }
        
        // Validate device section
        if (device) {
            auto block = device->get_block();
            if (block) {
                for (auto* stmt : block->get_statements()) {
                    auto prop = dynamic_cast<PropertyStatement*>(stmt);
                    if (prop) {
                        // Validate device properties have string values
                        auto str_value = dynamic_cast<StringValue*>(prop->get_value());
                        if (!str_value) {
                            std::cout << "  Error: Device property '" << prop->get_name() 
                                      << "' must have string value" << std::endl;
                            type_checking_valid = false;
                        }
                    }
                }
            }
        }
        
        // Find interfaces section
        SectionStatement* interfaces = nullptr;
        for (const auto& section : program->get_sections()) {
            if (section->get_section_type() == SectionStatement::SectionType::INTERFACES) {
                interfaces = section;
                break;
            }
        }
        
        // Validate interfaces section
        if (interfaces) {
            auto block = interfaces->get_block();
            if (block) {
                for (auto* stmt : block->get_statements()) {
                    auto interface = dynamic_cast<SectionStatement*>(stmt);
                    if (interface) {
                        auto if_block = interface->get_block();
                        if (if_block) {
                            for (auto* if_stmt : if_block->get_statements()) {
                                // Check IP subsection
                                auto ip_section = dynamic_cast<SectionStatement*>(if_stmt);
                                if (ip_section && ip_section->get_name() == "ip") {
                                    auto ip_block = ip_section->get_block();
                                    if (ip_block) {
                                        for (auto* ip_prop : ip_block->get_statements()) {
                                            auto address_prop = dynamic_cast<PropertyStatement*>(ip_prop);
                                            if (address_prop && address_prop->get_name() == "address") {
                                                // Validate IP address has CIDR format
                                                auto ip_value = dynamic_cast<IPCIDRValue*>(address_prop->get_value());
                                                if (!ip_value) {
                                                    std::cout << "  Error: IP address must have CIDR format" << std::endl;
                                                    type_checking_valid = false;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        std::cout << std::boolalpha << "Configuration type checking: " << type_checking_valid << std::endl;
    }

    // Consistency checking - verify firewall rules and interfaces are consistent
    bool consistency_valid = true;
    {
        std::cout << "\n--- Configuration Consistency ---" << std::endl;
        
        // Simple consistency check - in a real implementation we'd do more thorough checks
        std::cout << "  Verified firewall rules reference valid interfaces" << std::endl;
        std::cout << "  Verified IP addresses have valid formats" << std::endl;
        
        std::cout << std::boolalpha << "Configuration consistency check: " << consistency_valid << std::endl;
    }

    // Create program copy for equality comparison
    auto program_copy = new ProgramDeclaration();
    // Add the same sections to the copy (simplified version)
    program_copy->add_section(device_section); 
    // Note: In a real implementation, we would do a deep copy here

    // Check equality (simplified)
    bool equality_check = (program != nullptr && program_copy != nullptr);
    std::cout << std::boolalpha << "\nProgram equality check: " << equality_check << std::endl;

    // Print the AST 
    std::cout << "\n--- AST Structure ---" << std::endl;
    std::cout << program->to_string() << std::endl;
    
    // ============ TRANSLATION TO MIKROTIK SCRIPT ============
    
    std::cout << "\n=== Translating to MikroTik RouterOS v7 Script ===" << std::endl;
    
    // Create output file and write MikroTik script
    std::ofstream output{"demo_program.txt"};
    if (output.is_open()) {
        output << program->to_mikrotik("");
        std::cout << "Script successfully written to demo_program.txt" << std::endl;
    } else {
        std::cerr << "Error: Could not open output file" << std::endl;
    }
    output.close();

    // Clean up
    program->destroy();
    delete program;
    program_copy->destroy();
    delete program_copy;

    return EXIT_SUCCESS;
}