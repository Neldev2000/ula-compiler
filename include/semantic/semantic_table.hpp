#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <utility>

// Forward declaration for AST classes
class Datatype;
class Expression;
class Statement;
class Declaration;

// Represents a symbol entry in the symbol table
struct Symbol
{
    Datatype* type;
    std::string name;
    
    static std::shared_ptr<Symbol> build(Datatype* type, std::string_view name) noexcept;
};

// The symbol table maintains a stack of scopes for name resolution
class SymbolTable
{
public:
    using TableType = std::unordered_map<std::string, std::shared_ptr<Symbol>>;
    using TableStack = std::vector<TableType>;

    SymbolTable() noexcept;
    ~SymbolTable() noexcept;

    // Scope management
    void enter_scope() noexcept;
    bool exit_scope() noexcept;
    TableType::size_type scope_level() const noexcept;

    // Symbol binding and lookup
    bool bind(const std::string& name, std::shared_ptr<Symbol> symbol) noexcept;
    std::shared_ptr<Symbol> lookup(const std::string& name) noexcept;
    std::shared_ptr<Symbol> current_scope_lookup(const std::string& name) noexcept;

private:
    static std::shared_ptr<Symbol> find_in_scope(const std::string& name, const TableType& scope) noexcept;
    TableStack scopes;
};

// Semantic analysis functions
bool resolve_name_expression(Expression* expr, SymbolTable& symbol_table);
bool resolve_name_statement(Statement* stmt, SymbolTable& symbol_table);
bool resolve_name_declaration(Declaration* decl, SymbolTable& symbol_table);
bool resolve_name_body(const std::vector<Statement*>& body, SymbolTable& symbol_table);

// Type checking functions
std::pair<bool, Datatype*> expression_type_check(Expression* expr);
std::pair<bool, Datatype*> statement_type_check(Statement* stmt);
std::pair<bool, Datatype*> declaration_type_check(Declaration* decl);
std::pair<bool, Datatype*> body_type_check(const std::vector<Statement*>& body);
