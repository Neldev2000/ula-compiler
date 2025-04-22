
# Análisis Semántico para el DSL de Configuración Mikrotik

## Introducción

El Análisis Semántico constituye la tercera fase crítica en el proceso de compilación de nuestro Lenguaje Específico de Dominio (DSL) para configuración de dispositivos Mikrotik. Mientras que el análisis léxico identifica los tokens y el análisis sintáctico verifica la estructura gramatical, el análisis semántico se encarga de validar el significado y la coherencia de las construcciones del lenguaje. Esta fase garantiza que las configuraciones de red, aunque sintácticamente correctas, también sean semánticamente válidas dentro del contexto específico de las redes Mikrotik.

## Objetivos del Análisis Semántico

- **Verificación de Tipos**: Asegurar que los valores asignados a las propiedades sean del tipo correcto según el contexto.
- **Resolución de Nombres**: Verificar que todas las referencias a entidades correspondan a elementos previamente definidos.
- **Validación Contextual**: Comprobar que las configuraciones respeten las restricciones específicas del dominio de redes.
- **Detección de Errores Semánticos**: Identificar problemas como referencias circulares, redefiniciones o configuraciones contradictorias.
- **Preparación para la Generación de Código**: Recopilar y organizar la información necesaria para la fase final de generación de comandos Mikrotik.

## Arquitectura del Análisis Semántico

### Tabla de Símbolos

La tabla de símbolos constituye la estructura de datos central del análisis semántico, implementada en los archivos `semantic_table.hpp` y `semantic_table.cpp`. Esta estructura mantiene un registro de todas las entidades definidas en la configuración y sus propiedades asociadas.

```cpp
struct Symbol
{
    Datatype* type;
    std::string name;
    
    static std::shared_ptr<Symbol> build(Datatype* type, std::string_view name) noexcept;
};

class SymbolTable
{
public:
    using TableType = std::unordered_map<std::string, std::shared_ptr<Symbol>>;
    using TableStack = std::vector<TableType>;

    SymbolTable() noexcept;
    ~SymbolTable() noexcept;

    // Gestión de ámbitos
    void enter_scope() noexcept;
    bool exit_scope() noexcept;
    TableType::size_type scope_level() const noexcept;

    // Vinculación y búsqueda de símbolos
    bool bind(const std::string& name, std::shared_ptr<Symbol> symbol) noexcept;
    std::shared_ptr<Symbol> lookup(const std::string& name) noexcept;
    std::shared_ptr<Symbol> current_scope_lookup(const std::string& name) noexcept;

private:
    static std::shared_ptr<Symbol> find_in_scope(const std::string& name, const TableType& scope) noexcept;
    TableStack scopes;
};
```

La arquitectura de la tabla de símbolos proporciona:
- **Gestión de Ámbitos Jerárquicos**: Una pila de tablas hash que representan la estructura anidada de configuraciones
- **Vinculación Eficiente**: Métodos para registrar y buscar símbolos en diferentes ámbitos
- **Resolución Contextual**: Mecanismos para determinar la visibilidad de entidades según su contexto

### Sistema de Verificación de Tipos

El sistema de verificación de tipos opera sobre el AST decorado, utilizando una jerarquía de tipos específica para redes:

```cpp
std::pair<bool, Datatype*> expression_type_check(Expression* expr);
std::pair<bool, Datatype*> statement_type_check(Statement* stmt);
std::pair<bool, Datatype*> declaration_type_check(Declaration* decl);
std::pair<bool, Datatype*> body_type_check(const std::vector<Statement*>& body);
```

### Resolución de Nombres

La resolución de nombres implementa un recorrido recursivo del AST para validar la existencia de entidades referenciadas:

```cpp
bool resolve_name_expression(Expression* expr, SymbolTable& symbol_table);
bool resolve_name_statement(Statement* stmt, SymbolTable& symbol_table);
bool resolve_name_declaration(Declaration* decl, SymbolTable& symbol_table);
bool resolve_name_body(const std::vector<Statement*>& body, SymbolTable& symbol_table);
```

## Componentes del Análisis Semántico

### 1. Tabla de Símbolos (`semantic_table.hpp`, `semantic_table.cpp`)

#### 1.1 Clase Symbol

La clase `Symbol` representa una entrada individual en la tabla de símbolos:

```cpp
struct Symbol
{
    Datatype* type;          // Tipo asociado al símbolo
    std::string name;        // Nombre identificador único
    
    static std::shared_ptr<Symbol> build(Datatype* type, std::string_view name) noexcept;
};
```

Cada símbolo encapsula:
- Un apuntador al tipo de datos (`Datatype*`) que especifica el tipo semántico
- Un nombre identificador único (`std::string name`)
- Un método de fábrica estático para crear instancias de manera consistente

#### 1.2 Clase SymbolTable

La clase `SymbolTable` implementa una tabla de símbolos con ámbitos anidados:

```cpp
class SymbolTable
{
public:
    using TableType = std::unordered_map<std::string, std::shared_ptr<Symbol>>;
    using TableStack = std::vector<TableType>;

    // Métodos de gestión del ciclo de vida
    SymbolTable() noexcept;
    ~SymbolTable() noexcept;

    // Métodos de gestión de ámbitos
    void enter_scope() noexcept;
    bool exit_scope() noexcept;
    TableType::size_type scope_level() const noexcept;

    // Métodos de vinculación y búsqueda
    bool bind(const std::string& name, std::shared_ptr<Symbol> symbol) noexcept;
    std::shared_ptr<Symbol> lookup(const std::string& name) noexcept;
    std::shared_ptr<Symbol> current_scope_lookup(const std::string& name) noexcept;

private:
    static std::shared_ptr<Symbol> find_in_scope(const std::string& name, const TableType& scope) noexcept;
    TableStack scopes;       // Pila de ámbitos (tablas hash)
};
```

Características principales:
- **Estructura Anidada**: Utiliza una pila de tablas hash (`TableStack`) para representar ámbitos anidados
- **Gestión de Ámbitos**: Métodos para entrar y salir de ámbitos (`enter_scope()`, `exit_scope()`)
- **Vinculación de Símbolos**: Método `bind()` para registrar nuevos símbolos en el ámbito actual
- **Búsqueda Jerárquica**: Métodos para buscar símbolos en todos los ámbitos (`lookup()`) o solo en el ámbito actual (`current_scope_lookup()`)

### 2. Verificación de Tipos

#### 2.1 Sistema de Tipos

El sistema de tipos para el DSL de Mikrotik está basado en la jerarquía de tipos definida en `datatype.hpp`:

```cpp
class Datatype : public ASTNodeInterface
{
public:
    enum class Type {
        STRING,
        NUMBER,
        BOOLEAN,
        IP_ADDRESS,
        IP_CIDR,
        IP_RANGE,
        IPV6_ADDRESS,
        IPV6_CIDR,
        IPV6_RANGE,
        SECTION,
        LIST
    };

    Datatype(Type type_value) noexcept;
    Type get_type() const noexcept;
    virtual std::string type_name() const;
};
```

El sistema incluye:
- **Tipos Básicos**: `StringDatatype`, `NumberDatatype`, `BooleanDatatype`
- **Tipos de Red**: `IPAddressDatatype`, `IPCIDRDatatype`, `IPRangeDatatype`
- **Tipos Compuestos**: `ListDatatype` (para colecciones homogéneas)
- **Tipos Sección**: Representan contextos específicos como interfaces o reglas de firewall

#### 2.2 Funciones de Verificación de Tipos

La verificación de tipos se implementa mediante un conjunto de funciones recursivas:

```cpp
// Verifica el tipo de una expresión y devuelve el tipo resultante
std::pair<bool, Datatype*> expression_type_check(Expression* expr);

// Verifica los tipos en una sentencia
std::pair<bool, Datatype*> statement_type_check(Statement* stmt);

// Verifica los tipos en una declaración
std::pair<bool, Datatype*> declaration_type_check(Declaration* decl);

// Verifica los tipos en un bloque de sentencias
std::pair<bool, Datatype*> body_type_check(const std::vector<Statement*>& body);
```

Cada función:
- Recibe un nodo del AST como entrada
- Efectúa verificaciones específicas según el tipo de nodo
- Devuelve un par que indica éxito/fracaso y el tipo determinado

### 3. Resolución de Nombres

#### 3.1 Funciones de Resolución

La resolución de nombres se implementa mediante funciones especializadas que recorren el AST:

```cpp
// Resuelve nombres en expresiones
bool resolve_name_expression(Expression* expr, SymbolTable& symbol_table);

// Resuelve nombres en sentencias
bool resolve_name_statement(Statement* stmt, SymbolTable& symbol_table);

// Resuelve nombres en declaraciones
bool resolve_name_declaration(Declaration* decl, SymbolTable& symbol_table);

// Resuelve nombres en bloques de código
bool resolve_name_body(const std::vector<Statement*>& body, SymbolTable& symbol_table);
```

Estas funciones:
- Reciben un nodo del AST y la tabla de símbolos como parámetros
- Validan referencias a identificadores contra la tabla de símbolos
- Manejan la creación y destrucción de ámbitos según corresponda
- Devuelven un booleano indicando éxito o fracaso en la resolución

#### 3.2 Proceso de Resolución

La resolución sigue un enfoque post-orden:
1. Procesa los hijos del nodo actual
2. Resuelve referencias y valida identificadores en el contexto actual
3. Registra nuevas entidades en la tabla de símbolos según corresponda

### 4. Validaciones Específicas del Dominio

#### 4.1 Validaciones de Red

El análisis semántico incluye validaciones específicas para el dominio de configuración de redes:

```cpp
// Validación de dirección IP
bool validate_ip_address(const std::string& ip);

// Validación de notación CIDR
bool validate_ip_cidr(const std::string& cidr);

// Validación de interfaces
bool validate_interface_property(const std::string& property, Expression* value);

// Validación de reglas de firewall
bool validate_firewall_rule(Declaration* rule);
```

Estas funciones implementan reglas específicas del dominio como:
- Rangos válidos para IDs de VLAN (1-4094)
- Formatos correctos para direcciones IP y notación CIDR
- Combinaciones permitidas de propiedades según el tipo de entidad
- Validación de referencias entre entidades relacionadas

## Razones del Diseño Elegido

### Organización de Ámbitos Jerárquicos

La implementación utiliza una pila de tablas hash para representar ámbitos anidados por las siguientes razones:

1. **Correspondencia Natural**: Refleja la estructura anidada de las configuraciones de red Mikrotik
2. **Búsqueda Eficiente**: Las tablas hash proporcionan acceso O(1) a símbolos por nombre
3. **Gestión de Visibilidad**: Permite implementar reglas de shadowing y visibilidad de manera intuitiva
4. **Flexibilidad**: Soporta un número arbitrario de niveles de anidamiento

### Enfoque de Verificación de Tipos

El sistema adopta un enfoque recursivo post-orden para la verificación de tipos:

1. **Composicionalidad**: El tipo de una expresión compuesta se determina a partir de los tipos de sus componentes
2. **Detección Temprana**: Los errores se identifican tan pronto como sea posible durante el recorrido
3. **Anotación de Tipos**: Decora el AST con información de tipos para fases posteriores

### Gestión de Errores Orientada al Contexto

El sistema de gestión de errores está diseñado para proporcionar información contextual detallada:

1. **Mensajes Específicos**: Cada error incluye detalles precisos sobre su naturaleza
2. **Ubicación Exacta**: Se registra la posición del error para facilitar su localización
3. **Recuperación**: El análisis continúa después de encontrar errores para detectar múltiples problemas en una sola pasada

## Ejemplos de Uso

### Ejemplo 1: Verificación de Declaración de Interfaz

**Código DSL:**
```
interfaces:
    ether1:
        type = "ethernet"
        ip:
            address = 192.168.1.1/24
```

**Proceso de Análisis Semántico:**

1. **Construcción de Tabla de Símbolos**:
   ```
   Ámbito Global:
     - interfaces: SectionType

   Ámbito de interfaces:
     - ether1: InterfaceDeclaration

   Ámbito de ether1:
     - type: StringType
     - ip: SectionType

   Ámbito de ip:
     - address: IPCIDRType
   ```

2. **Verificación de Tipos**:
   - `type = "ethernet"`: Verifica que "ethernet" sea un valor válido para el tipo de interfaz
   - `address = 192.168.1.1/24`: Verifica que sea una notación CIDR válida

3. **Resolución de Nombres**:
   - Verifica que `interfaces` sea una sección válida
   - Registra `ether1` como interfaz en el ámbito de interfaces
   - Registra `ip` como subsección en el ámbito de ether1
   - Registra `address` como propiedad en el ámbito de ip

### Ejemplo 2: Detección de Error Semántico

**Código DSL con Error:**
```
interfaces:
    ether1:
        ip:
            address = "invalid_ip_address"
```

**Detección del Error:**
1. Durante la verificación de tipos, se determina que `"invalid_ip_address"` es un `StringValue`
2. Se verifica que la propiedad `address` en el contexto de `ip` espera un valor de tipo `IPCIDRType`
3. Se detecta incompatibilidad de tipos y se genera un error semántico
4. El error reportado incluye: ubicación, tipo esperado, tipo encontrado y sugerencia de corrección

## Integración con Otras Fases del Compilador

### Integración con el Análisis Sintáctico

El análisis semántico recibe el AST generado por el parser y lo procesa para:
1. Verificar la corrección semántica de la estructura sintáctica
2. Decorar los nodos con información de tipos y resolución de nombres
3. Detectar errores que no son detectables a nivel sintáctico

### Preparación para la Generación de Código

El análisis semántico prepara el código para la fase de generación:
1. Garantiza que todas las referencias sean válidas
2. Asegura que los tipos de datos sean compatibles con las operaciones
3. Proporciona un AST enriquecido con información semántica
4. Facilita optimizaciones específicas del dominio

## Conclusión

El análisis semántico constituye un componente crítico del compilador del DSL de configuración Mikrotik, asegurando que las configuraciones no solo sean sintácticamente correctas, sino también semánticamente válidas y significativas en el contexto de las redes. Mediante la implementación de una tabla de símbolos robusta, un sistema de verificación de tipos específico del dominio y mecanismos eficientes de resolución de nombres, esta fase contribuye significativamente a la calidad y confiabilidad del código generado.

La arquitectura modular y el diseño orientado a objetos facilitan la extensibilidad y mantenimiento del analizador semántico, permitiendo incorporar nuevas validaciones específicas del dominio a medida que evoluciona el lenguaje. Esta combinación de generalidad y especialización hace que el analizador semántico sea una herramienta poderosa para garantizar la corrección de las configuraciones de red Mikrotik antes de su implementación en dispositivos reales.
