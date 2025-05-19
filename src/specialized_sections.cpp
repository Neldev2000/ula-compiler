#include "specialized_sections.hpp"
#include "semantic_validator.hpp"
#include <sstream>
#include <algorithm>
#include <set>
#include <regex>

// SpecializedSection implementation
SpecializedSection::SpecializedSection(std::string_view name) noexcept
    : SectionStatement(name, SectionType::CUSTOM) // Temporarily set as CUSTOM, will be overridden
{
}

std::string SpecializedSection::to_mikrotik(const std::string& ident) const {
    // Common translation logic
    return translate_section(ident);
}

// DeviceSection implementation
DeviceSection::DeviceSection(std::string_view name) noexcept
    : SpecializedSection(name)
{
    // Override the section type
    this->type = SectionType::DEVICE;
}

std::tuple<bool, std::string> DeviceSection::validate() const noexcept {
    DeviceValidator validator;
    return validator.validate(get_block());

}

std::string DeviceSection::translate_section(const std::string& ident) const {
    std::string result = "# Device Configuration\n";
    
    if (get_block()) {
        // Extract device properties
        std::string vendor = "";
        std::string model = "";
        std::string hostname = "";
        
        // Iterate through statements to find vendor, model, and hostname properties
        const BlockStatement* block = get_block();
        for (const Statement* stmt : block->get_statements()) {
            const PropertyStatement* prop = dynamic_cast<const PropertyStatement*>(stmt);
            if (prop) {
                const std::string& name = prop->get_name();
                Expression* expr = prop->get_value();
                
                if (name == "vendor" && expr) {
                    const StringValue* value = dynamic_cast<const StringValue*>(expr);
                    if (value) {
                        // Remove any quotes from the string value
                        vendor = value->get_value();
                        if (vendor.size() >= 2 && vendor.front() == '"' && vendor.back() == '"') {
                            vendor = vendor.substr(1, vendor.size() - 2);
                        }
                    }
                }
                else if (name == "model" && expr) {
                    const StringValue* value = dynamic_cast<const StringValue*>(expr);
                    if (value) {
                        // Remove any quotes from the string value
                        model = value->get_value();
                        if (model.size() >= 2 && model.front() == '"' && model.back() == '"') {
                            model = model.substr(1, model.size() - 2);
                        }
                    }
                }
                else if (name == "hostname" && expr) {
                    const StringValue* value = dynamic_cast<const StringValue*>(expr);
                    if (value) {
                        // Remove any quotes from the string value
                        hostname = value->get_value();
                        if (hostname.size() >= 2 && hostname.front() == '"' && hostname.back() == '"') {
                            hostname = hostname.substr(1, hostname.size() - 2);
                        }
                    }
                }
            }
        }
        
        // Create the combined name: vendor_hostname_model
        std::string combined_name = "";
        if (!vendor.empty()) {
            combined_name += vendor;
            if (!hostname.empty() || !model.empty()) combined_name += "_";
        }
        
        if (!hostname.empty()) {
            combined_name += hostname;
            if (!model.empty()) combined_name += "_";
        }
        
        if (!model.empty()) {
            combined_name += model;
        }
        
        // If we couldn't extract the values, use a default name
        if (combined_name.empty()) {
            combined_name = "router";
        }
        
        // Generate the MikroTik script
        result += "/system identity set name=\"" + combined_name + "\"\n";
    }
    
    return result;
}


// InterfacesSection implementation
InterfacesSection::InterfacesSection(std::string_view name) noexcept
    : SpecializedSection(name)
{
    this->type = SectionType::INTERFACES;


}
std::tuple<bool, std::string> InterfacesSection::validate() const noexcept {
    InterfacesValidator validator;
    return validator.validate(get_block());
}



std::string InterfacesSection::translate_section(const std::string& ident) const {
    std::string result = "# Interface Configuration\n";

    if (get_block()) {
        const BlockStatement* block = get_block();
   
        
        // 1. First approach - look for subsections within our block (normal case)
        for (const Statement* stmt : block->get_statements()) {
            if (const SectionStatement* section = dynamic_cast<const SectionStatement*>(stmt)) {
                std::string interface_name = section->get_name();
              
                
                // Clean up interface name
                if (!interface_name.empty() && interface_name.back() == ':') {
                    interface_name = interface_name.substr(0, interface_name.size() - 1);
                }
                
                // If we somehow still have a colon in the name at this point, try to extract the name part
                size_t colon_pos = interface_name.find(':');
                if (colon_pos != std::string::npos) {
                    interface_name = interface_name.substr(0, colon_pos);
                }
                
                // Ensure interface name is valid
                if (interface_name.empty()) {
                    
                    continue; // Skip invalid interface names
                }
                
                // Process this interface using our helper
                result += process_interface_section(section, interface_name);
            }
        }
    }
    return result;
}

// Add helper method to process a single interface section
std::string InterfacesSection::process_interface_section(const SectionStatement* section, const std::string& interface_name) const {
    std::string result = "";
    
    if (!section || !section->get_block()) {
        return result;
    }
    
    const BlockStatement* interface_block = section->get_block();
    
    // Extract interface properties
    std::string type = "";
    std::string mtu = "";
    std::string disabled = "";
    std::string mac_address = "";
    std::string comment = "";
    std::string description = "";
    std::string vlan_id = "";
    std::string parent_interface = "";
    std::map<std::string, std::string> other_props;
    
    // Process all properties in the interface section
    for (const Statement* prop_stmt : interface_block->get_statements()) {
        const PropertyStatement* prop = dynamic_cast<const PropertyStatement*>(prop_stmt);
        if (prop) {
            const std::string& prop_name = prop->get_name();
            Expression* expr = prop->get_value();
            
            // Extract string value if possible
            std::string value = "";
            if (const StringValue* str_val = dynamic_cast<const StringValue*>(expr)) {
                value = str_val->get_value();
                // Remove quotes if present
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
            } else if (const NumberValue* num_val = dynamic_cast<const NumberValue*>(expr)) {
                value = std::to_string(num_val->get_value());
            } else if (const BooleanValue* bool_val = dynamic_cast<const BooleanValue*>(expr)) {
                value = bool_val->get_value() ? "yes" : "no";
            }
            
            // Store property values
            if (prop_name == "type") {
                type = value;
            } else if (prop_name == "mtu") {
                mtu = value;
            } else if (prop_name == "disabled" || prop_name == "admin_state") {
                // Map admin_state to disabled 
                if (value == "enabled") {
                    disabled = "no";  // not disabled = enabled
                } else if (value == "disabled") {
                    disabled = "yes"; // disabled = yes
                } else {
                    disabled = value; // use as-is if not a recognized value
                }
            } else if (prop_name == "mac_address" || prop_name == "mac") {
                mac_address = value;
            } else if (prop_name == "comment") {
                comment = value;
            } else if (prop_name == "description") {
                description = value;
            } else if (prop_name == "vlan_id") {
                vlan_id = value;
            } else if (prop_name == "interface") {
                parent_interface = value;
            } else {
                other_props[prop_name] = value;
            }
        }
    }
    
    // If description is set but comment is not, use description as comment
    if (comment.empty() && !description.empty()) {
        comment = description;
    }
    
    // Detect interface type if not explicitly specified
    if (type.empty()) {
        if (interface_name.find("ether") == 0) {
            type = "ethernet";
        } else if (interface_name.find("bridge") == 0) {
            type = "bridge";
        } else if (interface_name.find("vlan") == 0) {
            type = "vlan";
        } else if (interface_name.find("bond") == 0) {
            type = "bonding";
        } else if (interface_name.find("loop") == 0) {
            type = "loopback";
        }
    }
    
   
    
    // Generate commands based on interface type
    if (type == "ethernet") {
        result += "/interface ethernet set " + interface_name;
        if (!mtu.empty()) result += " mtu=" + mtu;
        if (!disabled.empty()) result += " disabled=" + disabled;
        if (!mac_address.empty()) result += " mac-address=" + mac_address;
        if (!comment.empty()) result += " comment=\"" + comment + "\"";
        
        // Add other ethernet-specific properties
        if (other_props.count("advertise")) 
            result += " advertise=" + other_props["advertise"];
        if (other_props.count("arp")) 
            result += " arp=" + other_props["arp"];
        
        result += "\n";
    } else if (type == "vlan") {
        result += "/interface vlan add";
        result += " name=" + interface_name;
        if (!vlan_id.empty()) result += " vlan-id=" + vlan_id;
        if (!parent_interface.empty()) result += " interface=" + parent_interface;
        if (!disabled.empty()) result += " disabled=" + disabled;
        if (!mtu.empty()) result += " mtu=" + mtu;
        if (!comment.empty()) result += " comment=\"" + comment + "\"";
        result += "\n";
    } else if (type == "bridge") {
        result += "/interface bridge add";
        result += " name=" + interface_name;
        if (!disabled.empty()) result += " disabled=" + disabled;
        if (!mtu.empty()) result += " mtu=" + mtu;
        if (!comment.empty()) result += " comment=\"" + comment + "\"";
        
        // Add other bridge-specific properties
        if (other_props.count("protocol-mode")) 
            result += " protocol-mode=" + other_props["protocol-mode"];
        if (other_props.count("fast-forward")) 
            result += " fast-forward=" + other_props["fast-forward"];
        
        result += "\n";
        
        // Add bridge ports if specified
        if (other_props.count("ports")) {
            std::string ports = other_props["ports"];
            // Remove brackets if present (assuming list format)
            if (ports.size() >= 2 && ports.front() == '[' && ports.back() == ']') {
                ports = ports.substr(1, ports.size() - 2);
            }
            
            // Split ports by comma
            size_t pos = 0;
            std::string port;
            while ((pos = ports.find(',')) != std::string::npos) {
                port = ports.substr(0, pos);
                if (!port.empty()) {
                    // Trim whitespace
                    port.erase(0, port.find_first_not_of(" \t"));
                    port.erase(port.find_last_not_of(" \t") + 1);
                    
                    result += "/interface bridge port add bridge=" + interface_name + " interface=" + port + "\n";
                }
                ports.erase(0, pos + 1);
            }
            
            // Add the last port
            if (!ports.empty()) {
                ports.erase(0, ports.find_first_not_of(" \t"));
                ports.erase(ports.find_last_not_of(" \t") + 1);
                
                result += "/interface bridge port add bridge=" + interface_name + " interface=" + ports + "\n";
            }
        }
    } else if (type == "loopback") {
        result += "/interface add name=" + interface_name + " type=loopback";
        if (!disabled.empty()) result += " disabled=" + disabled;
        if (!comment.empty()) result += " comment=\"" + comment + "\"";
        result += "\n";
    } else if (type == "bonding") {
        result += "/interface bonding add";
        result += " name=" + interface_name;
        if (!disabled.empty()) result += " disabled=" + disabled;
        if (!mtu.empty()) result += " mtu=" + mtu;
        if (!comment.empty()) result += " comment=\"" + comment + "\"";
        
        // Add other bonding-specific properties
        if (other_props.count("mode")) 
            result += " mode=" + other_props["mode"];
        if (other_props.count("slaves")) 
            result += " slaves=" + other_props["slaves"];
        
        result += "\n";
    } else {
        // Generic interface command
        result += "/interface set " + interface_name;
        if (!disabled.empty()) result += " disabled=" + disabled;
        if (!mtu.empty()) result += " mtu=" + mtu;
        if (!comment.empty()) result += " comment=\"" + comment + "\"";
        result += "\n";
    }
    
    // Add interface lists if specified
    if (other_props.count("lists")) {
        std::string lists = other_props["lists"];
        // Similar processing as for bridge ports
        if (lists.size() >= 2 && lists.front() == '[' && lists.back() == ']') {
            lists = lists.substr(1, lists.size() - 2);
        }
        
        size_t pos = 0;
        std::string list;
        while ((pos = lists.find(',')) != std::string::npos) {
            list = lists.substr(0, pos);
            if (!list.empty()) {
                list.erase(0, list.find_first_not_of(" \t"));
                list.erase(list.find_last_not_of(" \t") + 1);
                
                result += "/interface list member add list=" + list + " interface=" + interface_name + "\n";
            }
            lists.erase(0, pos + 1);
        }
        
        if (!lists.empty()) {
            lists.erase(0, lists.find_first_not_of(" \t"));
            lists.erase(lists.find_last_not_of(" \t") + 1);
            
            result += "/interface list member add list=" + lists + " interface=" + interface_name + "\n";
        }
    }
    
    return result;
}

// IPSection implementation
IPSection::IPSection(std::string_view name) noexcept
    : SpecializedSection(name)
{
    this->type = SectionType::IP;
}

std::tuple<bool, std::string> IPSection::validate() const noexcept {
    IPValidator validator;
    return validator.validate(get_block());
}

std::string IPSection::translate_section(const std::string& ident) const {
    std::string result = ident + "# IP Configuration: " + get_name() + "\n";
    
    if (get_block()) {
        const BlockStatement* block = get_block();
        
        // Process each statement in the IP section
        for (const auto* stmt : block->get_statements()) {
            // Check if this is a section (interface, route, firewall, etc.)
            if (const auto* subsection = dynamic_cast<const SectionStatement*>(stmt)) {
                std::string subsection_name = subsection->get_name();
                
                // Handle different IP subsections based on name
                if (subsection_name == "route" || subsection_name == "routes") {
                    // Handle IP routes
                    if (subsection->get_block()) {
                        for (const auto* route_stmt : subsection->get_block()->get_statements()) {
                            if (const auto* route_prop = dynamic_cast<const PropertyStatement*>(route_stmt)) {
                                if (route_prop->get_name() == "default" && route_prop->get_value()) {
                                    // Default route
                                    std::string gateway = route_prop->get_value()->to_mikrotik("");
                                    // Remove quotes if present
                                    if (gateway.size() >= 2 && gateway.front() == '"' && gateway.back() == '"') {
                                        gateway = gateway.substr(1, gateway.size() - 2);
                                    }
                                    result += "/ip route add dst-address=0.0.0.0/0 gateway=" + gateway + "\n";
                                }
                            } else if (const auto* route_section = dynamic_cast<const SectionStatement*>(route_stmt)) {
                                // Handle specific route entries
                                std::string dst_address = route_section->get_name();
                                std::string gateway = "";
                                std::string distance = "";
                                
                                if (route_section->get_block()) {
                                    for (const auto* route_detail : route_section->get_block()->get_statements()) {
                                        if (const auto* detail_prop = dynamic_cast<const PropertyStatement*>(route_detail)) {
                                            if (detail_prop->get_name() == "gateway" && detail_prop->get_value()) {
                                                gateway = detail_prop->get_value()->to_mikrotik("");
                                                // Remove quotes if present
                                                if (gateway.size() >= 2 && gateway.front() == '"' && gateway.back() == '"') {
                                                    gateway = gateway.substr(1, gateway.size() - 2);
                                                }
                                            } else if (detail_prop->get_name() == "distance" && detail_prop->get_value()) {
                                                distance = detail_prop->get_value()->to_mikrotik("");
                                            }
                                        }
                                    }
                                }
                                
                                if (!gateway.empty()) {
                                    result += "/ip route add dst-address=" + dst_address;
                                    result += " gateway=" + gateway;
                                    if (!distance.empty()) {
                                        result += " distance=" + distance;
                                    }
                                    result += "\n";
                                }
                            }
                        }
                    }
                } else if (subsection_name == "firewall") {
                    // Handle firewall rules
                    if (subsection->get_block()) {
                        for (const auto* fw_stmt : subsection->get_block()->get_statements()) {
                            if (const auto* fw_section = dynamic_cast<const SectionStatement*>(fw_stmt)) {
                                std::string chain_name = fw_section->get_name();
                                
                                // Process filter or nat chains
                                if (chain_name == "filter" || chain_name == "nat") {
                                    if (fw_section->get_block()) {
                                        for (const auto* rule_stmt : fw_section->get_block()->get_statements()) {
                                            if (const auto* rule_section = dynamic_cast<const SectionStatement*>(rule_stmt)) {
                                                std::string rule_chain = rule_section->get_name();
                                                std::string action = "";
                                                std::string protocol = "";
                                                std::string dst_port = "";
                                                std::string dst_address = "";
                                                std::string src_address = "";
                                                std::string out_interface = "";
                                                std::string in_interface = "";
                                                
                                                if (rule_section->get_block()) {
                                                    for (const auto* rule_prop : rule_section->get_block()->get_statements()) {
                                                        if (const auto* prop = dynamic_cast<const PropertyStatement*>(rule_prop)) {
                                                            std::string prop_name = prop->get_name();
                                                            std::string value = "";
                                                            if (prop->get_value()) {
                                                                value = prop->get_value()->to_mikrotik("");
                                                                // Remove quotes if present
                                                                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                                                                    value = value.substr(1, value.size() - 2);
                                                                }
                                                            }
                                                            
                                                            if (prop_name == "action") action = value;
                                                            else if (prop_name == "protocol") protocol = value;
                                                            else if (prop_name == "dst-port") dst_port = value;
                                                            else if (prop_name == "dst-address") dst_address = value;
                                                            else if (prop_name == "src-address") src_address = value;
                                                            else if (prop_name == "out-interface") out_interface = value;
                                                            else if (prop_name == "in-interface") in_interface = value;
                                                        }
                                                    }
                                                }
                                                
                                                // Generate firewall rule
                                                if (!action.empty()) {
                                                    result += "/ip firewall " + chain_name + " add chain=" + rule_chain;
                                                    result += " action=" + action;
                                                    if (!protocol.empty()) result += " protocol=" + protocol;
                                                    if (!dst_port.empty()) result += " dst-port=" + dst_port;
                                                    if (!dst_address.empty()) result += " dst-address=" + dst_address;
                                                    if (!src_address.empty()) result += " src-address=" + src_address;
                                                    if (!out_interface.empty()) result += " out-interface=" + out_interface;
                                                    if (!in_interface.empty()) result += " in-interface=" + in_interface;
                                                    result += "\n";
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else if (subsection_name == "dhcp-server") {
                    // Handle DHCP server configuration
                    if (subsection->get_block()) {
                        for (const auto* dhcp_stmt : subsection->get_block()->get_statements()) {
                            if (const auto* dhcp_section = dynamic_cast<const SectionStatement*>(dhcp_stmt)) {
                                std::string dhcp_name = dhcp_section->get_name();
                                std::string interface = "";
                                std::string address_pool = "";
                                std::string lease_time = "";
                                
                                if (dhcp_section->get_block()) {
                                    for (const auto* dhcp_prop : dhcp_section->get_block()->get_statements()) {
                                        if (const auto* prop = dynamic_cast<const PropertyStatement*>(dhcp_prop)) {
                                            std::string prop_name = prop->get_name();
                                            std::string value = "";
                                            if (prop->get_value()) {
                                                value = prop->get_value()->to_mikrotik("");
                                                // Remove quotes if present
                                                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                                                    value = value.substr(1, value.size() - 2);
                                                }
                                            }
                                            
                                            if (prop_name == "interface") interface = value;
                                            else if (prop_name == "address-pool") address_pool = value;
                                            else if (prop_name == "lease-time") lease_time = value;
                                        }
                                    }
                                }
                                
                                // Generate DHCP server
                                if (!interface.empty()) {
                                    result += "/ip dhcp-server add name=" + dhcp_name;
                                    result += " interface=" + interface;
                                    if (!address_pool.empty()) result += " address-pool=" + address_pool;
                                    if (!lease_time.empty()) result += " lease-time=" + lease_time;
                                    result += "\n";
                                }
                            }
                        }
                    }
                } else if (subsection_name == "dhcp-client") {
                    // Handle DHCP client configuration
                    if (subsection->get_block()) {
                        for (const auto* dhcp_stmt : subsection->get_block()->get_statements()) {
                            if (const auto* dhcp_prop = dynamic_cast<const PropertyStatement*>(dhcp_stmt)) {
                                std::string interface = dhcp_prop->get_name();
                                std::string disabled = "no"; // Enable by default
                                
                                if (dhcp_prop->get_value()) {
                                    std::string value = dhcp_prop->get_value()->to_mikrotik("");
                                    // Remove quotes if present
                                    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                                        value = value.substr(1, value.size() - 2);
                                    }
                                    
                                    if (value == "false" || value == "no") {
                                        disabled = "yes";
                                    }
                                }
                                
                                result += "/ip dhcp-client add interface=" + interface;
                                result += " disabled=" + disabled + "\n";
                            }
                        }
                    }
                } else if (subsection_name == "dns") {
                    // Handle DNS configuration
                    std::string servers = "";
                    std::string allow_remote = "";
                     
                    if (subsection->get_block()) {
                        for (const auto* dns_prop : subsection->get_block()->get_statements()) {
                            if (const auto* prop = dynamic_cast<const PropertyStatement*>(dns_prop)) {
                                std::string prop_name = prop->get_name();
                                std::string value = "";
                                if (prop->get_value()) {
                                    value = prop->get_value()->to_mikrotik("");
                                    // Remove quotes if present
                                    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                                        value = value.substr(1, value.size() - 2);
                                    }
                                }
                                
                                if (prop_name == "servers") servers = value;
                                else if (prop_name == "allow-remote-requests") allow_remote = value;
                            }
                        }
                    }
                    
                    // Generate DNS configuration
                    if (!servers.empty() || !allow_remote.empty()) {
                        result += "/ip dns set";
                        if (!servers.empty()) result += " servers=" + servers;
                        if (!allow_remote.empty()) result += " allow-remote-requests=" + allow_remote;
                        result += "\n";
                    }
                } else {
                    // Process as an interface with IP addresses (default case)
                    std::string interface_name = subsection_name;
                    
                    // Process the IP configuration for this interface
                    if (subsection->get_block()) {
                        for (const auto* ip_stmt : subsection->get_block()->get_statements()) {
                            if (const auto* ip_prop = dynamic_cast<const PropertyStatement*>(ip_stmt)) {
                                if (ip_prop->get_name() == "address" && ip_prop->get_value()) {
                                    std::string ip_value = ip_prop->get_value()->to_mikrotik("");
                                    // Remove quotes if present
                                    if (ip_value.size() >= 2 && ip_value.front() == '"' && ip_value.back() == '"') {
                                        ip_value = ip_value.substr(1, ip_value.size() - 2);
                                    }
                                    
                                    // Generate /ip address add command
                                    result += "/ip address add address=" + ip_value + 
                                              " interface=" + interface_name + "\n";
                                }
                            }
                        }
                    }
                }
            } else if (const auto* prop_stmt = dynamic_cast<const PropertyStatement*>(stmt)) {
                // Handle top-level IP properties (direct properties under the ip: section)
                // This could be for global IP settings or simple configurations
                std::string prop_name = prop_stmt->get_name();
                
                if (prop_name == "arp") {
                    // Handle static ARP entries
                    if (prop_stmt->get_value()) {
                        // Check if this is a section statement containing ARP entries
                        const SectionStatement* arp_section = dynamic_cast<const SectionStatement*>(prop_stmt->get_value());
                        if (arp_section && arp_section->get_block()) {
                            const BlockStatement* arp_block = arp_section->get_block();
                            for (const auto* arp_stmt : arp_block->get_statements()) {
                                if (const auto* arp_prop = dynamic_cast<const PropertyStatement*>(arp_stmt)) {
                                    std::string ip_address = arp_prop->get_name();
                                    std::string mac_address = "";
                                    std::string interface = "";
                                    
                                    // Handle the ARP entry properties
                                    if (arp_prop->get_value()) {
                                        // Try to get MAC and interface from nested section
                                        const SectionStatement* mac_section = dynamic_cast<const SectionStatement*>(arp_prop->get_value());
                                        if (mac_section && mac_section->get_block()) {
                                            for (const auto* mac_stmt : mac_section->get_block()->get_statements()) {
                                                if (const auto* mac_prop = dynamic_cast<const PropertyStatement*>(mac_stmt)) {
                                                    if (mac_prop->get_name() == "mac-address" && mac_prop->get_value()) {
                                                        mac_address = mac_prop->get_value()->to_mikrotik("");
                                                    } else if (mac_prop->get_name() == "interface" && mac_prop->get_value()) {
                                                        interface = mac_prop->get_value()->to_mikrotik("");
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    
                                    if (!mac_address.empty() && !interface.empty()) {
                                        // Remove quotes if present
                                        if (mac_address.size() >= 2 && mac_address.front() == '"' && mac_address.back() == '"') {
                                            mac_address = mac_address.substr(1, mac_address.size() - 2);
                                        }
                                        if (interface.size() >= 2 && interface.front() == '"' && interface.back() == '"') {
                                            interface = interface.substr(1, interface.size() - 2);
                                        }
                                        
                                        result += "/ip arp add address=" + ip_address;
                                        result += " mac-address=" + mac_address;
                                        result += " interface=" + interface + "\n";
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

// RoutingSection implementation
RoutingSection::RoutingSection(std::string_view name) noexcept
    : SpecializedSection(name)
{
    this->type = SectionType::ROUTING;
}

std::tuple<bool, std::string> RoutingSection::validate() const noexcept {
    RoutingValidator validator;
    return validator.validate(get_block());
}

std::string RoutingSection::translate_section(const std::string& ident) const {
    std::string result = ident + "# Routing Configuration: " + get_name() + "\n";
    
    if (get_block()) {
        const BlockStatement* block = get_block();
        
        // Process each statement in the routing section
        for (const auto* stmt : block->get_statements()) {
            // Handle properties vs subsections differently
            if (const auto* prop_stmt = dynamic_cast<const PropertyStatement*>(stmt)) {
                // Handle properties like default gateway
                std::string prop_name = prop_stmt->get_name();
                
                if (prop_name == "static_route_default_gw" && prop_stmt->get_value()) {
                    // Default route
                    std::string gateway = prop_stmt->get_value()->to_mikrotik("");
                    // Remove quotes if present
                    if (gateway.size() >= 2 && gateway.front() == '"' && gateway.back() == '"') {
                        gateway = gateway.substr(1, gateway.size() - 2);
                    }
                    
                    // Generate default route
                    result += "/ip route add dst-address=0.0.0.0/0 gateway=" + gateway + "\n";
                }
            } else if (const auto* route_section = dynamic_cast<const SectionStatement*>(stmt)) {
                // Handle named route sections (static_route1, etc.)
                std::string route_name = route_section->get_name();
                
                // Extract route properties
                std::string destination = "";
                std::string gateway = "";
                std::string distance = "";
                std::string routing_table = "";
                std::string check_gateway = "";
                std::string scope = "";
                std::string target_scope = "";
                bool suppress_hw_offload = false;
                
                if (route_section->get_block()) {
                    for (const auto* route_prop : route_section->get_block()->get_statements()) {
                        if (const auto* prop = dynamic_cast<const PropertyStatement*>(route_prop)) {
                            std::string prop_name = prop->get_name();
                            std::string value = "";
                            
                            if (prop->get_value()) {
                                value = prop->get_value()->to_mikrotik("");
                                // Remove quotes if present
                                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                                    value = value.substr(1, value.size() - 2);
                                }
                            }
                            
                            if (prop_name == "destination" || prop_name == "dst-address" || prop_name == "dst") {
                                destination = value;
                            } else if (prop_name == "gateway" || prop_name == "gw") {
                                gateway = value;
                            } else if (prop_name == "distance") {
                                distance = value;
                            } else if (prop_name == "routing-table" || prop_name == "table") {
                                routing_table = value;
                            } else if (prop_name == "check-gateway") {
                                check_gateway = value;
                            } else if (prop_name == "scope") {
                                scope = value;
                            } else if (prop_name == "target-scope") {
                                target_scope = value;
                            } else if (prop_name == "suppress-hw-offload") {
                                suppress_hw_offload = (value == "yes" || value == "true");
                            }
                        }
                    }
                }
                
                // Generate a static route if we have at least a destination and gateway
                if (!destination.empty() && !gateway.empty()) {
                    result += "/ip route add dst-address=" + destination;
                    result += " gateway=" + gateway;
                    
                    // Add optional parameters
                    if (!distance.empty()) {
                        result += " distance=" + distance;
                    }
                    if (!routing_table.empty()) {
                        result += " routing-table=" + routing_table;
                    }
                    if (!check_gateway.empty()) {
                        result += " check-gateway=" + check_gateway;
                    }
                    if (!scope.empty()) {
                        result += " scope=" + scope;
                    }
                    if (!target_scope.empty()) {
                        result += " target-scope=" + target_scope;
                    }
                    if (suppress_hw_offload) {
                        result += " suppress-hw-offload=yes";
                    }
                    
                    result += "\n";
                }
            } else if (const auto* subsection = dynamic_cast<const SectionStatement*>(stmt)) {
                // Handle specific routing subsections like 'table', 'rule', etc.
                std::string subsection_name = subsection->get_name();
                
                if (subsection_name == "table" || subsection_name == "tables") {
                    // Handle routing tables
                    if (subsection->get_block()) {
                        for (const auto* table_stmt : subsection->get_block()->get_statements()) {
                            if (const auto* table_section = dynamic_cast<const SectionStatement*>(table_stmt)) {
                                std::string table_name = table_section->get_name();
                                bool fib = true; // Default in RouterOS v7
                                
                                if (table_section->get_block()) {
                                    for (const auto* table_prop : table_section->get_block()->get_statements()) {
                                        if (const auto* prop = dynamic_cast<const PropertyStatement*>(table_prop)) {
                                            if (prop->get_name() == "fib" && prop->get_value()) {
                                                std::string value = prop->get_value()->to_mikrotik("");
                                                if (value == "no" || value == "false") {
                                                    fib = false;
                                                }
                                            }
                                        }
                                    }
                                }
                                
                                // Generate routing table
                                result += "/routing table add name=" + table_name;
                                if (fib) {
                                    result += " fib";
                                }
                                result += "\n";
                            }
                        }
                    }
                } else if (subsection_name == "rule" || subsection_name == "rules") {
                    // Handle routing rules
                    if (subsection->get_block()) {
                        for (const auto* rule_stmt : subsection->get_block()->get_statements()) {
                            if (const auto* rule_section = dynamic_cast<const SectionStatement*>(rule_stmt)) {
                                std::string rule_name = rule_section->get_name();
                                std::string src_address = "";
                                std::string dst_address = "";
                                std::string interface = "";
                                std::string action = "";
                                std::string table = "";
                                
                                if (rule_section->get_block()) {
                                    for (const auto* rule_prop : rule_section->get_block()->get_statements()) {
                                        if (const auto* prop = dynamic_cast<const PropertyStatement*>(rule_prop)) {
                                            std::string prop_name = prop->get_name();
                                            std::string value = "";
                                            
                                            if (prop->get_value()) {
                                                value = prop->get_value()->to_mikrotik("");
                                                // Remove quotes if present
                                                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                                                    value = value.substr(1, value.size() - 2);
                                                }
                                            }
                                            
                                            if (prop_name == "src-address") {
                                                src_address = value;
                                            } else if (prop_name == "dst-address") {
                                                dst_address = value;
                                            } else if (prop_name == "interface") {
                                                interface = value;
                                            } else if (prop_name == "action") {
                                                action = value;
                                            } else if (prop_name == "table") {
                                                table = value;
                                            }
                                        }
                                    }
                                }
                                
                                // Generate routing rule
                                result += "/routing rule add";
                                if (!src_address.empty()) {
                                    result += " src-address=" + src_address;
                                }
                                if (!dst_address.empty()) {
                                    result += " dst-address=" + dst_address;
                                }
                                if (!interface.empty()) {
                                    result += " interface=" + interface;
                                }
                                if (!action.empty()) {
                                    result += " action=" + action;
                                }
                                if (!table.empty()) {
                                    result += " table=" + table;
                                }
                                result += "\n";
                            }
                        }
                    }
                } else if (subsection_name == "filter") {
                    // Handle routing filters for v7
                    if (subsection->get_block()) {
                        for (const auto* filter_stmt : subsection->get_block()->get_statements()) {
                            if (const auto* filter_section = dynamic_cast<const SectionStatement*>(filter_stmt)) {
                                std::string chain_name = filter_section->get_name();
                                std::string rule = "";
                                
                                if (filter_section->get_block()) {
                                    for (const auto* filter_prop : filter_section->get_block()->get_statements()) {
                                        if (const auto* prop = dynamic_cast<const PropertyStatement*>(filter_prop)) {
                                            if (prop->get_name() == "rule" && prop->get_value()) {
                                                rule = prop->get_value()->to_mikrotik("");
                                                // Remove quotes if present
                                                if (rule.size() >= 2 && rule.front() == '"' && rule.back() == '"') {
                                                    rule = rule.substr(1, rule.size() - 2);
                                                }
                                                
                                                // Generate routing filter rule
                                                result += "/routing/filter/rule add chain=" + chain_name;
                                                result += " rule=\"" + rule + "\"\n";
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
    
    return result;
}

// FirewallSection implementation
FirewallSection::FirewallSection(std::string_view name) noexcept
    : SpecializedSection(name)
{
    this->type = SectionType::FIREWALL;
}

std::tuple<bool, std::string> FirewallSection::validate() const noexcept {
    FirewallValidator validator;
    return validator.validate(get_block());
}

std::string FirewallSection::translate_section(const std::string& ident) const {
    std::string result = ident + "# Firewall Configuration: " + get_name() + "\n";
    
    if (get_block()) {
        const BlockStatement* block = get_block();
        
        // Process each subsection (filter, nat, etc.)
        for (const auto* stmt : block->get_statements()) {
            if (const auto* section = dynamic_cast<const SectionStatement*>(stmt)) {
                std::string section_name = section->get_name();
                
                // Process filter rules
                if (section_name == "filter") {
                    if (section->get_block()) {
                        for (const auto* rule_stmt : section->get_block()->get_statements()) {
                            if (const auto* rule = dynamic_cast<const SectionStatement*>(rule_stmt)) {
                                std::string rule_name = rule->get_name();
                                std::string chain = "forward"; // Default chain
                                std::string action = "";
                                std::string connection_state = "";
                                std::string protocol = "";
                                std::string src_address = "";
                                std::string dst_address = "";
                                std::string src_port = "";
                                std::string dst_port = "";
                                std::string in_interface = "";
                                std::string out_interface = "";
                                std::string comment = rule_name;
                                
                                // Extract properties for this filter rule
                                if (rule->get_block()) {
                                    for (const auto* prop_stmt : rule->get_block()->get_statements()) {
                                        if (const auto* prop = dynamic_cast<const PropertyStatement*>(prop_stmt)) {
                                            std::string prop_name = prop->get_name();
                                            std::string value = "";
                                            
                                            if (prop->get_value()) {
                                                value = prop->get_value()->to_mikrotik("");
                                                // Remove quotes if present
                                                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                                                    value = value.substr(1, value.size() - 2);
                                                }
                                            }
                                            
                                            if (prop_name == "chain") {
                                                chain = value;
                                            } else if (prop_name == "action") {
                                                action = value;
                                            } else if (prop_name == "connection_state" || prop_name == "connection-state") {
                                                // Handle array of states like ["established", "related"]
                                                if (value.front() == '[' && value.back() == ']') {
                                                    value = value.substr(1, value.size() - 2);
                                                    std::string state;
                                                    std::stringstream ss(value);
                                                    bool first = true;
                                                    
                                                    while (ss >> state) {
                                                        // Clean up state - remove quotes and commas
                                                        state.erase(remove(state.begin(), state.end(), '"'), state.end());
                                                        state.erase(remove(state.begin(), state.end(), ','), state.end());
                                                        
                                                        if (!state.empty()) {
                                                            if (first) {
                                                                connection_state = state;
                                                                first = false;
                                                            } else {
                                                                connection_state += "," + state;
                                                            }
                                                        }
                                                    }
                                                } else {
                                                    connection_state = value;
                                                }
                                            } else if (prop_name == "protocol") {
                                                protocol = value;
                                            } else if (prop_name == "src_address" || prop_name == "src-address") {
                                                src_address = value;
                                            } else if (prop_name == "dst_address" || prop_name == "dst-address") {
                                                dst_address = value;
                                            } else if (prop_name == "src_port" || prop_name == "src-port") {
                                                src_port = value;
                                            } else if (prop_name == "dst_port" || prop_name == "dst-port") {
                                                dst_port = value;
                                            } else if (prop_name == "in_interface" || prop_name == "in-interface") {
                                                in_interface = value;
                                            } else if (prop_name == "out_interface" || prop_name == "out-interface") {
                                                out_interface = value;
                                            } else if (prop_name == "comment") {
                                                comment = value;
                                            }
                                        }
                                    }
                                }
                                
                                // Generate the filter rule if an action is specified
                                if (!action.empty()) {
                                    result += "/ip firewall filter add chain=" + chain + " action=" + action;
                                    
                                    // Add optional parameters
                                    if (!connection_state.empty()) {
                                        // Clean up connection_state - remove quotes and braces
                                        std::string clean_conn_state;
                                        bool in_quote = false;
                                        
                                        for (size_t i = 0; i < connection_state.size(); i++) {
                                            char c = connection_state[i];
                                            // Skip braces, quotes, and spaces
                                            if (c == '{' || c == '}' || c == '"' || (c == ' ' && !in_quote)) {
                                                continue;
                                            }
                                            clean_conn_state += c;
                                        }
                                        
                                        result += " connection-state=" + clean_conn_state;
                                    }
                                    if (!protocol.empty()) {
                                        result += " protocol=" + protocol;
                                    }
                                    if (!src_address.empty()) {
                                        result += " src-address=" + src_address;
                                    }
                                    if (!dst_address.empty()) {
                                        result += " dst-address=" + dst_address;
                                    }
                                    if (!src_port.empty()) {
                                        result += " src-port=" + src_port;
                                    }
                                    if (!dst_port.empty()) {
                                        result += " dst-port=" + dst_port;
                                    }
                                    if (!in_interface.empty()) {
                                        result += " in-interface=" + in_interface;
                                    }
                                    if (!out_interface.empty()) {
                                        result += " out-interface=" + out_interface;
                                    }
                                    if (!comment.empty()) {
                                        result += " comment=\"" + comment + "\"";
                                    }
                                    
                                    result += "\n";
                                }
                            }
                        }
                    }
                }
                // Process NAT rules
                else if (section_name == "nat") {
                    if (section->get_block()) {
                        for (const auto* rule_stmt : section->get_block()->get_statements()) {
                            if (const auto* rule = dynamic_cast<const SectionStatement*>(rule_stmt)) {
                                std::string rule_name = rule->get_name();
                                std::string chain = "srcnat"; // Default chain
                                std::string action = "";
                                std::string protocol = "";
                                std::string src_address = "";
                                std::string dst_address = "";
                                std::string src_port = "";
                                std::string dst_port = "";
                                std::string in_interface = "";
                                std::string out_interface = "";
                                std::string to_addresses = "";
                                std::string to_ports = "";
                                std::string comment = rule_name;
                                
                                // Extract properties for this NAT rule
                                if (rule->get_block()) {
                                    for (const auto* prop_stmt : rule->get_block()->get_statements()) {
                                        if (const auto* prop = dynamic_cast<const PropertyStatement*>(prop_stmt)) {
                                            std::string prop_name = prop->get_name();
                                            std::string value = "";
                                            
                                            if (prop->get_value()) {
                                                value = prop->get_value()->to_mikrotik("");
                                                // Remove quotes if present
                                                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                                                    value = value.substr(1, value.size() - 2);
                                                }
                                            }
                                            
                                            if (prop_name == "chain") {
                                                chain = value;
                                            } else if (prop_name == "action") {
                                                action = value;
                                            } else if (prop_name == "protocol") {
                                                protocol = value;
                                            } else if (prop_name == "src_address" || prop_name == "src-address") {
                                                src_address = value;
                                            } else if (prop_name == "dst_address" || prop_name == "dst-address") {
                                                dst_address = value;
                                            } else if (prop_name == "src_port" || prop_name == "src-port") {
                                                src_port = value;
                                            } else if (prop_name == "dst_port" || prop_name == "dst-port") {
                                                dst_port = value;
                                            } else if (prop_name == "in_interface" || prop_name == "in-interface") {
                                                in_interface = value;
                                            } else if (prop_name == "out_interface" || prop_name == "out-interface") {
                                                out_interface = value;
                                            } else if (prop_name == "to_addresses" || prop_name == "to-addresses") {
                                                to_addresses = value;
                                            } else if (prop_name == "to_ports" || prop_name == "to-ports") {
                                                to_ports = value;
                                            } else if (prop_name == "comment") {
                                                comment = value;
                                            }
                                        }
                                    }
                                }
                                
                                // Generate the NAT rule if an action is specified
                                if (!action.empty()) {
                                    result += "/ip firewall nat add chain=" + chain + " action=" + action;
                                    
                                    // Add optional parameters
                                    if (!protocol.empty()) {
                                        result += " protocol=" + protocol;
                                    }
                                    if (!src_address.empty()) {
                                        result += " src-address=" + src_address;
                                    }
                                    if (!dst_address.empty()) {
                                        result += " dst-address=" + dst_address;
                                    }
                                    if (!src_port.empty()) {
                                        result += " src-port=" + src_port;
                                    }
                                    if (!dst_port.empty()) {
                                        result += " dst-port=" + dst_port;
                                    }
                                    if (!in_interface.empty()) {
                                        result += " in-interface=" + in_interface;
                                    }
                                    if (!out_interface.empty()) {
                                        result += " out-interface=" + out_interface;
                                    }
                                    if (!to_addresses.empty() && action != "masquerade") {
                                        result += " to-addresses=" + to_addresses;
                                    }
                                    if (!to_ports.empty()) {
                                        result += " to-ports=" + to_ports;
                                    }
                                    if (!comment.empty()) {
                                        result += " comment=\"" + comment + "\"";
                                    }
                                    
                                    result += "\n";
                                }
                            }
                        }
                    }
                }
                // Process address-list rules (for blocking lists, etc.)
                else if (section_name == "address-list") {
                    if (section->get_block()) {
                        for (const auto* list_stmt : section->get_block()->get_statements()) {
                            if (const auto* list = dynamic_cast<const SectionStatement*>(list_stmt)) {
                                std::string list_name = list->get_name();
                                
                                // Process each address in the list
                                if (list->get_block()) {
                                    for (const auto* addr_stmt : list->get_block()->get_statements()) {
                                        if (const auto* addr_prop = dynamic_cast<const PropertyStatement*>(addr_stmt)) {
                                            std::string address = addr_prop->get_name();
                                            std::string comment = "";
                                            std::string timeout = "";
                                            
                                            if (addr_prop->get_value()) {
                                                // Check if it's a simple string (comment) or a block with properties
                                                std::string value = addr_prop->get_value()->to_mikrotik("");
                                                // Remove quotes if present
                                                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                                                    value = value.substr(1, value.size() - 2);
                                                    comment = value;
                                                }
                                            }
                                            
                                            // Generate address-list entry
                                            result += "/ip firewall address-list add list=" + list_name;
                                            result += " address=" + address;
                                            if (!comment.empty()) {
                                                result += " comment=\"" + comment + "\"";
                                            }
                                            if (!timeout.empty()) {
                                                result += " timeout=" + timeout;
                                            }
                                            result += "\n";
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                // Process service-port rules
                else if (section_name == "service-port") {
                    if (section->get_block()) {
                        for (const auto* service_stmt : section->get_block()->get_statements()) {
                            if (const auto* service_prop = dynamic_cast<const PropertyStatement*>(service_stmt)) {
                                std::string service_name = service_prop->get_name();
                                std::string value = "";
                                
                                if (service_prop->get_value()) {
                                    value = service_prop->get_value()->to_mikrotik("");
                                    // Remove quotes if present
                                    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                                        value = value.substr(1, value.size() - 2);
                                    }
                                }
                                
                                // Generate service-port setting
                                if (value == "yes" || value == "true") {
                                    result += "/ip firewall service-port set " + service_name + " disabled=no\n";
                                } else if (value == "no" || value == "false") {
                                    result += "/ip firewall service-port set " + service_name + " disabled=yes\n";
                                }
                            }
                        }
                    }
                }
                // Process raw rules (advanced firewall)
                else if (section_name == "raw") {
                    if (section->get_block()) {
                        for (const auto* rule_stmt : section->get_block()->get_statements()) {
                            if (const auto* rule = dynamic_cast<const SectionStatement*>(rule_stmt)) {
                                std::string rule_name = rule->get_name();
                                std::string chain = "prerouting"; // Default chain
                                std::string action = "";
                                std::string protocol = "";
                                std::string src_address = "";
                                std::string dst_address = "";
                                std::string comment = rule_name;
                                
                                // Extract properties for this raw rule
                                if (rule->get_block()) {
                                    for (const auto* prop_stmt : rule->get_block()->get_statements()) {
                                        if (const auto* prop = dynamic_cast<const PropertyStatement*>(prop_stmt)) {
                                            std::string prop_name = prop->get_name();
                                            std::string value = "";
                                            
                                            if (prop->get_value()) {
                                                value = prop->get_value()->to_mikrotik("");
                                                // Remove quotes if present
                                                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                                                    value = value.substr(1, value.size() - 2);
                                                }
                                            }
                                            
                                            if (prop_name == "chain") {
                                                chain = value;
                                            } else if (prop_name == "action") {
                                                action = value;
                                            } else if (prop_name == "protocol") {
                                                protocol = value;
                                            } else if (prop_name == "src_address" || prop_name == "src-address") {
                                                src_address = value;
                                            } else if (prop_name == "dst_address" || prop_name == "dst-address") {
                                                dst_address = value;
                                            } else if (prop_name == "comment") {
                                                comment = value;
                                            }
                                        }
                                    }
                                }
                                
                                // Generate the raw rule if an action is specified
                                if (!action.empty()) {
                                    result += "/ip firewall raw add chain=" + chain + " action=" + action;
                                    
                                    // Add optional parameters
                                    if (!protocol.empty()) {
                                        result += " protocol=" + protocol;
                                    }
                                    if (!src_address.empty()) {
                                        result += " src-address=" + src_address;
                                    }
                                    if (!dst_address.empty()) {
                                        result += " dst-address=" + dst_address;
                                    }
                                    if (!comment.empty()) {
                                        result += " comment=\"" + comment + "\"";
                                    }
                                    
                                    result += "\n";
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

// SystemSection implementation
SystemSection::SystemSection(std::string_view name) noexcept
    : DeviceSection(name)
{
    this->type = SectionType::SYSTEM;
}

// CustomSection implementation
CustomSection::CustomSection(std::string_view name) noexcept
    : SpecializedSection(name)
{
    this->type = SectionType::CUSTOM;
}

std::tuple<bool, std::string> CustomSection::validate() const noexcept {
    const BlockStatement* block = get_block();
    if (!block) return {false, "Custom section is missing a block statement"};
    
    // Custom sections are more permissive
    return {true, ""};
}

std::string CustomSection::translate_section(const std::string& ident) const {
    std::string result = ident + "# Custom Configuration: " + get_name() + "\n";
    
    if (get_block()) {
        // For custom sections, simply translate the block
        result += get_block()->to_mikrotik(ident);
    }
    
    return result;
}

// Factory function implementation
SpecializedSection* create_specialized_section(std::string_view name, SectionStatement::SectionType type) {
    switch (type) {
        case SectionStatement::SectionType::DEVICE:
            return new DeviceSection(name);
        case SectionStatement::SectionType::INTERFACES:
            return new InterfacesSection(name);
        case SectionStatement::SectionType::IP:
            return new IPSection(name);
        case SectionStatement::SectionType::ROUTING:
            return new RoutingSection(name);
        case SectionStatement::SectionType::FIREWALL:
            return new FirewallSection(name);
        case SectionStatement::SectionType::SYSTEM:
            return new SystemSection(name);
        case SectionStatement::SectionType::CUSTOM:
        default:
            return new CustomSection(name);
    }
} 