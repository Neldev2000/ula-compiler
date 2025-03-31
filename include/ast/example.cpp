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
    // Create the program (root) declaration
    auto program = new ProgramDeclaration();
    
    // ---- DEVICE SECTION ----
    // Create the device section with block
    auto device_block = new BlockStatement();
    device_block->add_statement(new PropertyStatement("vendor", new StringValue("mikrotik")));
    device_block->add_statement(new PropertyStatement("model", new StringValue("CCR2004-1G-12S+2XS")));
    
    auto device_section = new SectionStatement("device", SectionStatement::SectionType::DEVICE, device_block);
    program->add_section(device_section);
    
    // ---- INTERFACES SECTION ----
    // Create the interfaces section with block
    auto interfaces_block = new BlockStatement();
    
    // Create ether1 interface
    auto ether1_block = new BlockStatement();
    ether1_block->add_statement(new PropertyStatement("type", new StringValue("ethernet")));
    ether1_block->add_statement(new PropertyStatement("admin_state", new StringValue("enabled")));
    
    // Create IP subsection for ether1
    auto ip_block = new BlockStatement();
    ip_block->add_statement(new PropertyStatement("address", new IPCIDRValue("192.168.1.1/24")));
    
    auto ip_section = new SectionStatement("ip", SectionStatement::SectionType::IP, ip_block);
    ether1_block->add_statement(ip_section);
    
    auto ether1_section = new SectionStatement("ether1", SectionStatement::SectionType::CUSTOM, ether1_block);
    interfaces_block->add_statement(ether1_section);
    
    // Create ether2 interface
    auto ether2_block = new BlockStatement();
    ether2_block->add_statement(new PropertyStatement("type", new StringValue("ethernet")));
    ether2_block->add_statement(new PropertyStatement("admin_state", new StringValue("enabled")));
    ether2_block->add_statement(new PropertyStatement("description", new StringValue("WAN Connection")));
    
    auto ether2_section = new SectionStatement("ether2", SectionStatement::SectionType::CUSTOM, ether2_block);
    interfaces_block->add_statement(ether2_section);
    
    auto interfaces_section = new SectionStatement("interfaces", SectionStatement::SectionType::INTERFACES, interfaces_block);
    program->add_section(interfaces_section);
    
    // ---- FIREWALL SECTION ----
    // Create the firewall section with block
    auto firewall_block = new BlockStatement();
    
    // Create filter subsection
    auto filter_block = new BlockStatement();
    
    // Create rule
    auto rule_block = new BlockStatement();
    rule_block->add_statement(new PropertyStatement("chain", new StringValue("input")));
    
    // Create connection states list
    ValueList conn_states;
    conn_states.push_back(new StringValue("established"));
    conn_states.push_back(new StringValue("related"));
    auto conn_states_list = new ListValue(conn_states);
    
    rule_block->add_statement(new PropertyStatement("connection_state", conn_states_list));
    rule_block->add_statement(new PropertyStatement("action", new StringValue("accept")));
    
    auto rule_section = new SectionStatement("input_accept_established", 
                                            SectionStatement::SectionType::CUSTOM, 
                                            rule_block);
    filter_block->add_statement(rule_section);
    
    auto filter_section = new SectionStatement("filter", 
                                              SectionStatement::SectionType::CUSTOM, 
                                              filter_block);
    firewall_block->add_statement(filter_section);
    
    auto firewall_section = new SectionStatement("firewall", 
                                               SectionStatement::SectionType::FIREWALL, 
                                               firewall_block);
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