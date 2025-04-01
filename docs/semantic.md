# Análisis Semántico para el DSL de Configuración Mikrotik

## Introducción

El análisis semántico constituye la tercera fase crítica en el proceso de compilación de nuestro Lenguaje Específico de Dominio (DSL) para configuración de dispositivos Mikrotik. Mientras que el análisis léxico identifica los tokens y el análisis sintáctico verifica la estructura gramatical, el análisis semántico se encarga de validar el significado y la coherencia de las construcciones del lenguaje. Esta fase garantiza que las configuraciones de red, aunque sintácticamente correctas, también sean semánticamente válidas dentro del contexto de las redes Mikrotik.

## Objetivos del Análisis Semántico

- **Verificación de Tipos**: Asegurar que los valores asignados a las propiedades sean del tipo correcto.
- **Resolución de Nombres**: Verificar que todas las referencias a entidades (interfaces, secciones, etc.) correspondan a elementos previamente definidos.
- **Validación Contextual**: Comprobar que las configuraciones respeten las restricciones específicas del dominio de redes.
- **Detección de Errores Semánticos**: Identificar problemas como referencias circulares, redefiniciones o configuraciones contradictorias.
- **Preparación para la Generación de Código**: Recopilar y organizar la información necesaria para la fase final de generación de comandos Mikrotik.

## Tabla de Símbolos

### Diseño e Implementación

La tabla de símbolos constituye la estructura de datos central del análisis semántico, implementada en los archivos `semantic_table.hpp` y `semantic_table.cpp`. Esta estructura mantiene un registro de todas las entidades definidas en la configuración y sus propiedades asociadas.

#### Componentes Fundamentales

##### Clase Symbol

```cpp
struct Symbol
{
    Datatype* type;
    std::string name;
    
    static std::shared_ptr<Symbol> build(Datatype* type, std::string_view name) noexcept;
};
```

La clase `Symbol` representa una entrada en la tabla de símbolos con:
- Un tipo de datos (`Datatype`) que especifica el tipo de la entidad
- Un nombre identificador único
- Un método de fábrica `build()` para crear instancias de manera consistente

##### Clase SymbolTable

```cpp
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

La clase `SymbolTable` implementa la tabla de símbolos con las siguientes características:
- **Estructura Anidada**: Utiliza una pila de tablas hash (`TableStack`) para representar ámbitos anidados
- **Gestión de Ámbitos**: Métodos para entrar y salir de ámbitos (`enter_scope()`, `exit_scope()`)
- **Vinculación de Símbolos**: Método `bind()` para registrar nuevos símbolos en el ámbito actual
- **Búsqueda de Símbolos**: Métodos para buscar símbolos en todos los ámbitos (`lookup()`) o solo en el ámbito actual (`current_scope_lookup()`)

### Organización de Ámbitos

La tabla de símbolos implementa una gestión de ámbitos jerárquica que refleja la estructura anidada de las configuraciones de red:

1. **Ámbito Global**: Contiene secciones principales como `device`, `interfaces`, `ip`, etc.
2. **Ámbitos de Sección**: Cada sección crea un nuevo ámbito anidado
3. **Ámbitos de Subsección**: Las configuraciones anidadas como interfaces específicas o reglas de firewall

Esta estructura permite:
- **Shadowing Controlado**: Un identificador en un ámbito interno puede ocultar uno del mismo nombre en un ámbito externo
- **Validación Contextual**: Cada entidad solo es visible en su ámbito apropiado
- **Resolución Jerárquica**: La búsqueda de símbolos sigue la cadena de ámbitos desde el más interno al más externo

## Resolución de Nombres

La resolución de nombres es el proceso mediante el cual se verifica que todas las referencias a entidades en la configuración correspondan a elementos previamente definidos. Esta funcionalidad está implementada mediante una serie de funciones especializadas:

```cpp
bool resolve_name_expression(Expression* expr, SymbolTable& symbol_table);
bool resolve_name_statement(Statement* stmt, SymbolTable& symbol_table);
bool resolve_name_declaration(Declaration* decl, SymbolTable& symbol_table);
bool resolve_name_body(const std::vector<Statement*>& body, SymbolTable& symbol_table);
```

### Proceso de Resolución

El proceso de resolución de nombres sigue un enfoque recursivo, descendiendo por el árbol AST y validando cada referencia:

1. Para **expresiones** (`resolve_name_expression`):
   - Valida referencias a identificadores buscándolos en la tabla de símbolos
   - Procesa expresiones compuestas como listas o referencias a propiedades

2. Para **declaraciones** (`resolve_name_declaration`):
   - Crea un nuevo ámbito para declaraciones que definen su propio contexto
   - Registra las entidades declaradas en la tabla de símbolos
   - Valida las expresiones contenidas en la declaración

3. Para **sentencias** (`resolve_name_statement`):
   - Procesa propiedades y sus valores
   - Maneja bloques de sentencias y secciones
   - Enlaza con las funciones apropiadas para cada tipo de sentencia

4. Para **cuerpos de bloque** (`resolve_name_body`):
   - Itera a través de todas las sentencias en el bloque
   - Aplica la resolución de nombres a cada sentencia

### Ejemplo de Resolución

Para una configuración como:

```
interfaces:
    ether1:
        ip:
            address = 192.168.1.1/24
```

El proceso de resolución:
1. Registra `interfaces` en el ámbito global
2. Crea un ámbito anidado para `interfaces`
3. Registra `ether1` en el ámbito de `interfaces`
4. Crea un ámbito anidado para `ether1`
5. Registra `ip` en el ámbito de `ether1`
6. Procesa la propiedad `address` en el ámbito de `ip`

## Verificación de Tipos

La verificación de tipos asegura que los valores asignados a las propiedades sean del tipo correcto según el contexto. Está implementada mediante las siguientes funciones:

```cpp
std::pair<bool, Datatype*> expression_type_check(Expression* expr);
std::pair<bool, Datatype*> statement_type_check(Statement* stmt);
std::pair<bool, Datatype*> declaration_type_check(Declaration* decl);
std::pair<bool, Datatype*> body_type_check(const std::vector<Statement*>& body);
```

### Sistema de Tipos

El sistema de tipos para el DSL de Mikrotik se basa en la jerarquía de clases definida en `datatype.hpp`:

- **Tipos Básicos**:
  - `StringDatatype`: Para nombres, descripciones, etc.
  - `NumberDatatype`: Para números de puerto, IDs de VLAN, etc.
  - `BooleanDatatype`: Para estados habilitado/deshabilitado
  - `IPAddressDatatype`: Para direcciones IP
  - `IPCIDRDatatype`: Para notación CIDR

- **Tipos Compuestos**:
  - `ListDatatype`: Para listas de valores del mismo tipo

### Proceso de Verificación

La verificación de tipos sigue un enfoque similar a la resolución de nombres, recorriendo recursivamente el AST:

1. Para **expresiones** (`expression_type_check`):
   - Determina el tipo de literales (string, número, IP)
   - Para listas, verifica que todos los elementos sean del mismo tipo
   - Retorna el tipo calculado y un indicador de éxito

2. Para **declaraciones** (`declaration_type_check`):
   - Verifica los tipos de las propiedades en declaraciones
   - Asegura la compatibilidad entre valores y sus contextos esperados

3. Para **sentencias** (`statement_type_check`):
   - Procesa asignaciones de propiedades verificando la compatibilidad de tipos
   - Maneja bloques y secciones para verificaciones contextuales

4. Para **cuerpos de bloque** (`body_type_check`):
   - Itera a través de todas las sentencias verificando cada una
   - Propaga errores de tipo encontrados en cualquier sentencia

### Ejemplo de Verificación de Tipos

Para una propiedad como `address = 192.168.1.1/24`:

1. Se determina que `192.168.1.1/24` es un valor de tipo `IPCIDRDatatype`
2. Se verifica que la propiedad `address` en el contexto de `ip` espera un valor de tipo compatible con `IPCIDRDatatype`
3. Si los tipos son compatibles, la verificación tiene éxito; de lo contrario, se reporta un error de tipo

## Optimizaciones Semánticas

Además de la validación, el análisis semántico realiza optimizaciones específicas para el dominio de redes:

### 1. Inferencia de Tipos

Para casos donde no se especifica explícitamente el tipo:

```cpp
// Ejemplo simplificado de inferencia de tipos
Datatype* infer_type_from_context(const std::string& property_name, SectionType section_type) {
    if (section_type == SectionType::IP && property_name == "address") {
        return new IPCIDRDatatype();
    }
    // Otras reglas de inferencia...
    return new StringDatatype(); // Tipo por defecto
}
```

### 2. Validación Especializada

Para restricciones específicas del dominio de redes:

```cpp
// Ejemplo conceptual de validación especializada
bool validate_interface_property(const std::string& property_name, Expression* value, InterfaceType iface_type) {
    if (iface_type == InterfaceType::ETHERNET && property_name == "speed") {
        // Validar que el valor sea uno de los permitidos para velocidad ethernet
        auto* str_val = dynamic_cast<StringValue*>(value);
        if (!str_val) return false;
        
        return str_val->get_value() == "10Mbps" || 
               str_val->get_value() == "100Mbps" || 
               str_val->get_value() == "1Gbps";
    }
    // Otras validaciones...
    return true;
}
```

## Gestión de Errores Semánticos

El análisis semántico detecta diversos tipos de errores que no serían capturados por las fases léxica y sintáctica:

### Tipos de Errores Semánticos

1. **Referencia a Entidad No Declarada**:
   ```
   interfaces:
       ether1:
           link_to = ether2  # Error si ether2 no está definido
   ```

2. **Tipo Incompatible**:
   ```
   interfaces:
       ether1:
           admin_state = 123  # Error: se espera string "enabled" o "disabled"
   ```

3. **Redefinición de Entidad**:
   ```
   interfaces:
       ether1:
           # ...
       ether1:  # Error: redefinición de ether1
           # ...
   ```

4. **Valor Fuera de Rango**:
   ```
   interfaces:
       vlan1:
           vlan_id = 5000  # Error: VLAN ID debe estar entre 1 y 4094
   ```

5. **Referencia Circular**:
   ```
   routing:
       route1:
           next_hop = route2
       route2:
           next_hop = route1  # Error: referencia circular
   ```

### Estrategia de Reporte de Errores

Los errores semánticos se reportan con información contextual detallada:

- **Ubicación Precisa**: Referencia a la línea y columna del error
- **Descripción Clara**: Explicación concisa del problema
- **Sugerencia de Corrección**: Orientación sobre cómo solucionar el error
- **Contexto**: Información sobre el ámbito y entidades relacionadas

## Integración con las Fases del Compilador

El análisis semántico se integra con las demás fases del compilador de la siguiente manera:

### 1. Integración con el Análisis Sintáctico

- Recibe el AST generado por el parser
- Utiliza la estructura del AST para realizar el análisis semántico
- Enriquece el AST con información de tipos y símbolos

### 2. Preparación para la Generación de Código

- Produce un AST decorado con información semántica
- Proporciona una tabla de símbolos completa con todas las entidades y sus propiedades
- Garantiza que el programa sea semánticamente válido antes de la generación de código

## Ejemplo de Proceso Completo

Para ilustrar el proceso completo de análisis semántico, consideremos el siguiente fragmento de configuración:

```
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
```

### 1. Construcción de la Tabla de Símbolos

```
Ámbito Global:
  - interfaces: SectionType

Ámbito de interfaces:
  - ether1: InterfaceDeclaration
  - ether2: InterfaceDeclaration

Ámbito de ether1:
  - type: StringType ("ethernet")
  - admin_state: StringType ("enabled")
  - ip: SectionType

Ámbito de ip en ether1:
  - address: IPCIDRType (192.168.1.1/24)

Ámbito de ether2:
  - type: StringType ("ethernet")
  - admin_state: StringType ("enabled")
  - description: StringType ("WAN Connection")
```

### 2. Verificación de Tipos

- `type = "ethernet"`: String esperado ✓
- `admin_state = "enabled"`: String (de conjunto enumerado) esperado ✓
- `address = 192.168.1.1/24`: IPCIDR esperado ✓
- `description = "WAN Connection"`: String esperado ✓

### 3. Validación Contextual

- Verifica que "ethernet" sea un tipo válido de interfaz ✓
- Verifica que "enabled" sea un estado válido ✓
- Verifica que 192.168.1.1/24 sea una notación CIDR válida ✓

## Conclusión

El análisis semántico constituye una fase crítica en el compilador del DSL de configuración Mikrotik, garantizando la corrección semántica de las configuraciones de red más allá de su validez sintáctica. Mediante la implementación de una tabla de símbolos robusta, mecanismos eficientes de resolución de nombres y un sistema de verificación de tipos, esta fase asegura que las configuraciones sean no solo sintácticamente correctas sino también significativas y válidas en el contexto de las redes Mikrotik.

Este enfoque permite a los administradores de red identificar errores semánticos en sus configuraciones antes de aplicarlas a dispositivos reales, evitando problemas potenciales y mejorando la confiabilidad de las implementaciones. La integración del análisis semántico con las fases previas del compilador y su preparación para la generación de código final completan un pipeline de compilación robusto y eficiente para el DSL de configuración Mikrotik. 