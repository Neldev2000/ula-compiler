#include "semantic_table.hpp"
#include "../ast/datatype.hpp"
#include "../ast/declaration.hpp"
#include "../ast/expression.hpp"
#include "../ast/statement.hpp"

// Symbol implementation
std::shared_ptr<Symbol> Symbol::build(Datatype* type, std::string_view name) noexcept
{
    auto symbol = std::make_shared<Symbol>();
    symbol->type = type;
    symbol->name = name;
    return symbol;
}

// SymbolTable implementation
SymbolTable::SymbolTable() noexcept
{
    this->enter_scope();
}

SymbolTable::~SymbolTable() noexcept
{
    while (!this->scopes.empty()) {
        this->exit_scope();
    }
}

void SymbolTable::enter_scope() noexcept
{
    this->scopes.push_back(TableType{});
}

bool SymbolTable::exit_scope() noexcept
{
    if (this->scopes.empty())
    {
        return false;
    }

    this->scopes.pop_back();
    return true;
}

SymbolTable::TableType::size_type SymbolTable::scope_level() const noexcept
{
    return this->scopes.size();
}

bool SymbolTable::bind(const std::string& name, std::shared_ptr<Symbol> symbol) noexcept
{
    if (this->scopes.empty())
    {
        return false;
    }

    TableType& current_scope = this->scopes.back();

    if (SymbolTable::find_in_scope(name, current_scope) != nullptr)
    {
        return false;
    }

    current_scope.emplace(name, symbol);
    return true;
}

std::shared_ptr<Symbol> SymbolTable::lookup(const std::string& name) noexcept
{
    for (auto it = this->scopes.rbegin(); it != this->scopes.rend(); ++it)
    {
        auto found = SymbolTable::find_in_scope(name, *it);

        if (found != nullptr)
        {
            return found;
        }
    }

    return nullptr;
}

std::shared_ptr<Symbol> SymbolTable::current_scope_lookup(const std::string& name) noexcept
{
    if (this->scopes.empty())
    {
        return nullptr;
    }
    
    return SymbolTable::find_in_scope(name, this->scopes.back());
}

std::shared_ptr<Symbol> SymbolTable::find_in_scope(const std::string& name, const TableType& scope) noexcept
{
    auto found_it = scope.find(name);
        
    if (found_it != scope.end())
    {
        return found_it->second;
    }

    return nullptr;
}

// The following simplified implementations are specialized for the Mikrotik DSL
// rather than a general programming language, focusing on configuration validation

// Name resolution implementations
bool resolve_name_expression(Expression* expr, SymbolTable& symbol_table)
{
    if (!expr) return false;

    // For this DSL, expressions are typically simple string references or values
    // We'll provide a simplified name resolution that just reports success
    // In a real implementation, we would validate section/interface references
    
    // For string values, no name resolution needed
    if (dynamic_cast<StringValue*>(expr)) {
        return true;
    }
    
    // For IP values, no name resolution needed
    if (dynamic_cast<IPCIDRValue*>(expr)) {
        return true;
    }
    
    // For list values, check each item
    if (auto list_value = dynamic_cast<ListValue*>(expr)) {
        for (auto& item : list_value->get_values()) {
            if (!resolve_name_expression(item, symbol_table)) {
                return false;
            }
        }
        return true;
    }
    
    // Default to true for simplicity
    return true;
}

bool resolve_name_statement(Statement* stmt, SymbolTable& symbol_table)
{
    if (!stmt) return false;

    // Case: Property statement (key = value)
    if (auto prop_stmt = dynamic_cast<PropertyStatement*>(stmt)) {
        return resolve_name_expression(prop_stmt->get_value(), symbol_table);
    }
    
    // Case: Block statement (collection of statements)
    if (auto block_stmt = dynamic_cast<BlockStatement*>(stmt)) {
        for (auto* substmt : block_stmt->get_statements()) {
            if (!resolve_name_statement(substmt, symbol_table)) {
                return false;
            }
        }
        return true;
    }
    
    // Case: Section statement
    if (auto section_stmt = dynamic_cast<SectionStatement*>(stmt)) {
        // Check the statements inside this section
        if (section_stmt->get_block()) {
            return resolve_name_statement(section_stmt->get_block(), symbol_table);
        }
        return true;
    }
    
    // Case: Declaration statement
    if (auto decl_stmt = dynamic_cast<DeclarationStatement*>(stmt)) {
        return resolve_name_declaration(decl_stmt->get_declaration(), symbol_table);
    }

    return false;
}

bool resolve_name_declaration(Declaration* decl, SymbolTable& symbol_table)
{
    if (!decl) return false;

    // Case: Config declaration
    if (auto config_decl = dynamic_cast<ConfigDeclaration*>(decl)) {
        for (auto* stmt : config_decl->get_statements()) {
            if (!resolve_name_statement(stmt, symbol_table)) {
                return false;
            }
        }
        return true;
    }
    
    // Case: Property declaration
    if (auto prop_decl = dynamic_cast<PropertyDeclaration*>(decl)) {
        return resolve_name_expression(prop_decl->get_value(), symbol_table);
    }
    
    // Case: Interface declaration
    if (auto iface_decl = dynamic_cast<InterfaceDeclaration*>(decl)) {
        for (auto* stmt : iface_decl->get_statements()) {
            if (!resolve_name_statement(stmt, symbol_table)) {
                return false;
            }
        }
        return true;
    }
    
    // Case: Program declaration
    if (auto program_decl = dynamic_cast<ProgramDeclaration*>(decl)) {
        for (auto* section : program_decl->get_sections()) {
            symbol_table.enter_scope();
            bool result = resolve_name_statement(section, symbol_table);
            symbol_table.exit_scope();
            if (!result) {
                return false;
            }
        }
        return true;
    }

    return false;
}

bool resolve_name_body(const std::vector<Statement*>& body, SymbolTable& symbol_table)
{
    for (auto* stmt : body) {
        if (!resolve_name_statement(stmt, symbol_table)) {
            return false;
        }
    }
    
    return true;
}

// Type checking implementations
std::pair<bool, Datatype*> expression_type_check(Expression* expr)
{
    if (!expr) return {false, nullptr};

    // Case: String value
    if (auto str_expr = dynamic_cast<StringValue*>(expr)) {
        return {true, new StringDatatype()};
    }
    
    // Case: Number value
    if (auto num_expr = dynamic_cast<NumberValue*>(expr)) {
        return {true, new NumberDatatype()};
    }
    
    // Case: IP CIDR value
    if (auto ip_expr = dynamic_cast<IPCIDRValue*>(expr)) {
        return {true, new IPCIDRDatatype()};
    }
    
    // Case: List value
    if (auto list_expr = dynamic_cast<ListValue*>(expr)) {
        if (list_expr->get_values().empty()) {
            return {true, new ListDatatype(new StringDatatype())}; // Default empty list type
        }
        
        // Get type of first element and assume all elements are the same type
        auto first_type = expression_type_check(list_expr->get_values()[0]);
        if (!first_type.first) {
            return {false, nullptr};
        }
        
        // Check that all other elements match the first type
        for (size_t i = 1; i < list_expr->get_values().size(); i++) {
            auto elem_type = expression_type_check(list_expr->get_values()[i]);
            if (!elem_type.first || elem_type.second->get_type() != first_type.second->get_type()) {
                if (elem_type.second) delete elem_type.second;
                delete first_type.second;
                return {false, nullptr};
            }
            delete elem_type.second;
        }
        
        return {true, new ListDatatype(first_type.second)};
    }
    
    // Default fallback
    return {false, nullptr};
}

std::pair<bool, Datatype*> statement_type_check(Statement* stmt)
{
    if (!stmt) return {false, nullptr};

    // Case: Property statement
    if (auto prop_stmt = dynamic_cast<PropertyStatement*>(stmt)) {
        return expression_type_check(prop_stmt->get_value());
    }
    
    // Case: Block statement - just return success for blocks
    if (dynamic_cast<BlockStatement*>(stmt)) {
        return {true, nullptr};
    }
    
    // Case: Section statement - just return section type
    if (dynamic_cast<SectionStatement*>(stmt)) {
        auto type = new BasicDatatype(Datatype::Type::SECTION);
        return {true, type};
    }
    
    // Case: Declaration statement
    if (auto decl_stmt = dynamic_cast<DeclarationStatement*>(stmt)) {
        return declaration_type_check(decl_stmt->get_declaration());
    }
    
    // Statements that don't have a meaningful type
    return {true, nullptr};
}

std::pair<bool, Datatype*> declaration_type_check(Declaration* decl)
{
    if (!decl) return {false, nullptr};

    // Case: Config declaration - just return success for configurations
    if (dynamic_cast<ConfigDeclaration*>(decl)) {
        return {true, new BasicDatatype(Datatype::Type::SECTION)};
    }
    
    // Case: Property declaration - type is the type of the value
    if (auto prop_decl = dynamic_cast<PropertyDeclaration*>(decl)) {
        return expression_type_check(prop_decl->get_value());
    }
    
    // Case: Interface declaration - special interface type
    if (dynamic_cast<InterfaceDeclaration*>(decl)) {
        return {true, new BasicDatatype(Datatype::Type::SECTION)};
    }
    
    // Case: Program declaration - overall program has no specific type
    if (dynamic_cast<ProgramDeclaration*>(decl)) {
        return {true, nullptr};
    }
    
    return {false, nullptr};
}

std::pair<bool, Datatype*> body_type_check(const std::vector<Statement*>& body)
{
    Datatype* last_type = nullptr;
    
    for (auto* stmt : body) {
        if (last_type) {
            delete last_type;
            last_type = nullptr;
        }
        
        auto stmt_type = statement_type_check(stmt);
        if (!stmt_type.first) {
            if (stmt_type.second) delete stmt_type.second;
            return {false, nullptr};
        }
        
        last_type = stmt_type.second;
    }
    
    return {true, last_type};
}
