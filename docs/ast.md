# Árbol de Sintaxis Abstracta (AST) para el DSL de Configuración Mikrotik

## Introducción

El Árbol de Sintaxis Abstracta (AST) es una estructura de datos fundamental en el proceso de compilación que representa el código fuente de manera jerárquica y estructurada. Este documento detalla el diseño e implementación del AST para nuestro Lenguaje Específico de Dominio (DSL) orientado a la configuración de dispositivos Mikrotik. El AST permite transformar la estructura sintáctica del código en una representación lógica que facilita las fases posteriores de análisis semántico y generación de código.

## Objetivos del Diseño del AST

- **Representación Precisa**: Capturar fielmente la estructura jerárquica de las configuraciones de red Mikrotik.
- **Validación Semántica**: Facilitar la verificación de tipos y validaciones semánticas específicas del dominio de redes.
- **Extensibilidad**: Permitir la incorporación de nuevas construcciones de lenguaje sin modificar la estructura base.
- **Mantenibilidad**: Diseño modular y orientado a objetos que facilita el mantenimiento y la comprensión del código.
- **Optimización**: Estructura eficiente en memoria y rendimiento para el procesamiento de configuraciones complejas.

## Arquitectura del AST

### Interfaz Base: ASTNodeInterface

Todos los nodos del AST heredan de una interfaz común que define comportamientos fundamentales:

```cpp
class ASTNodeInterface
{
public:
    virtual ~ASTNodeInterface() noexcept;
    
    // Método para destruir apropiadamente el nodo y sus hijos
    virtual void destroy() noexcept = 0;
    
    // Método para generar representación textual (útil para depuración)
    virtual std::string to_string() const = 0;
};
```

Esta interfaz establece un contrato uniforme para todos los nodos, garantizando:
- **Gestión de Recursos**: A través del método `destroy()` que implementa la liberación de memoria
- **Introspección**: Mediante el método `to_string()` que facilita la depuración y visualización

### Categorías Principales de Nodos

El AST se estructura en cuatro categorías fundamentales:

1. **Expresiones**: Representan valores, variables y operaciones que evalúan a un valor
2. **Declaraciones**: Definen entidades nombradas como interfaces o secciones
3. **Sentencias**: Representan acciones o comandos dentro de la configuración
4. **Tipos de Datos**: Representan el sistema de tipos del lenguaje

## Componentes del AST

### 1. Expresiones (`expression.hpp`)

Las expresiones representan valores o construcciones que computan valores. La jerarquía incluye:

#### 1.1 Clase Base: Expression

```cpp
class Expression : public ASTNodeInterface
{
public:
    virtual Datatype* get_type() const = 0;
};
```

#### 1.2 Valores Literales

Derivan de la clase base `Value`:

- **StringValue**: Cadenas de texto (ej. "mikrotik")
- **NumberValue**: Valores numéricos (ej. 24)
- **BooleanValue**: Valores booleanos (true/false)
- **IPAddressValue**: Direcciones IP (ej. 192.168.1.1)
- **IPCIDRValue**: Notación CIDR (ej. 192.168.1.0/24)

#### 1.3 Estructuras Compuestas

- **ListValue**: Listas de valores (ej. ["established", "related"])
- **IdentifierExpression**: Referencias a entidades nombradas
- **PropertyReference**: Acceso a propiedades (ej. interface.address)

### 2. Sentencias (`statement.hpp`)

Las sentencias representan acciones o comandos en la configuración:

#### 2.1 Clase Base: Statement

```cpp
class Statement : public ASTNodeInterface
{
    // Clase base vacía
};
```

#### 2.2 Tipos de Sentencias

- **PropertyStatement**: Asignaciones de propiedades (ej. address = 192.168.1.1/24)
- **BlockStatement**: Colecciones de sentencias
- **SectionStatement**: Secciones de configuración nombradas con tipo específico
- **DeclarationStatement**: Envuelve declaraciones para incluirlas en bloques

#### 2.3 Secciones Específicas

La clase `SectionStatement` admite diferentes tipos de secciones:

```cpp
enum class SectionType {
    DEVICE,
    INTERFACES,
    IP,
    ROUTING,
    FIREWALL,
    SYSTEM,
    CUSTOM
};
```

### 3. Declaraciones (`declaration.hpp`)

Las declaraciones definen entidades nombradas en la configuración:

#### 3.1 Clase Base: Declaration

```cpp
class Declaration : public ASTNodeInterface
{
public:
    Declaration(std::string_view decl_name) noexcept;
    const std::string& get_name() const noexcept;
    
protected:
    std::string name;
};
```

#### 3.2 Tipos de Declaraciones

- **ConfigDeclaration**: Secciones de configuración
- **PropertyDeclaration**: Definiciones de propiedades
- **InterfaceDeclaration**: Definiciones de interfaces de red
- **ProgramDeclaration**: Nodo raíz que representa un programa completo

### 4. Tipos de Datos (`datatype.hpp`)

El sistema de tipos refleja los tipos de datos específicos para configuraciones de red:

#### 4.1 Clase Base: Datatype

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

#### 4.2 Tipos Específicos

- **BasicDatatype**: Tipos simples (String, Number, Boolean)
- **IPAddressDatatype**: Tipo para direcciones IP
- **IPCIDRDatatype**: Tipo para notación CIDR
- **ListDatatype**: Listas de elementos del mismo tipo

## Razones del Diseño Elegido

### Enfoque Orientado a Objetos vs. Alternativas

El AST implementa un diseño orientado a objetos con jerarquías de herencia por razones específicas:

1. **Seguridad de Tipos**: La herencia garantiza verificación de tipos en tiempo de compilación.
2. **Modelado del Dominio**: Las abstracciones representan directamente conceptos del dominio de configuración de redes.
3. **Evolución Controlada**: Facilita la extensión del lenguaje respetando el principio de diseño abierto/cerrado.
4. **Polimorfismo**: Permite operaciones genéricas sobre diferentes tipos de nodos.

### Gestión de Memoria

El diseño adopta un modelo explícito de gestión de memoria:

- **Punteros Simples**: Se utilizan para representar relaciones padre-hijo.
- **Semántica de Propiedad**: Cada nodo es responsable de destruir sus hijos.
- **Método destroy()**: Implementa el patrón de destrucción recursiva.

### Alternativa al Patrón Visitor

Aunque muchas implementaciones de AST utilizan el patrón Visitor, nuestro diseño opta por métodos virtuales porque:

1. **Simplicidad**: Reduce la complejidad del código y las dependencias.
2. **Flexibilidad**: Cada operación puede definir su propia estrategia de recorrido.
3. **Acoplamiento Reducido**: Permite añadir nuevas operaciones sin modificar las clases existentes.

## Ejemplos de Representación

### Ejemplo 1: Configuración Básica de Interfaz

**Código DSL:**
```
interfaces:
    ether1:
        type = "ethernet"
        ip:
            address = 192.168.1.1/24
```

**Representación en AST:**
1. `ProgramDeclaration` (raíz)
2. └── `SectionStatement` (tipo: INTERFACES)
3.     └── `BlockStatement`
4.         └── `InterfaceDeclaration` (nombre: "ether1")
5.             ├── `PropertyStatement` (nombre: "type", valor: StringValue("ethernet"))
6.             └── `SectionStatement` (tipo: IP)
7.                 └── `BlockStatement`
8.                     └── `PropertyStatement` (nombre: "address", valor: IPCIDRValue("192.168.1.1/24"))

### Ejemplo 2: Regla de Firewall

**Código DSL:**
```
firewall:
    filter:
        input_accept_established:
            chain = "input"
            connection_state = ["established", "related"]
            action = "accept"
```

**Representación en AST:**
1. `ProgramDeclaration` (raíz)
2. └── `SectionStatement` (tipo: FIREWALL)
3.     └── `BlockStatement`
4.         └── `SectionStatement` (tipo: CUSTOM, nombre: "filter")
5.             └── `BlockStatement`
6.                 └── `ConfigDeclaration` (nombre: "input_accept_established")
7.                     ├── `PropertyStatement` (nombre: "chain", valor: StringValue("input"))
8.                     ├── `PropertyStatement` (nombre: "connection_state", 
                          valor: ListValue([StringValue("established"), StringValue("related")]))
9.                     └── `PropertyStatement` (nombre: "action", valor: StringValue("accept"))

## Integración con el Análisis Semántico

El AST está diseñado para facilitar el análisis semántico mediante:

1. **Información de Tipos**: Cada expresión puede informar sobre su tipo a través del método `get_type()`.
2. **Estructura Jerárquica**: Permite la implementación eficiente de análisis de ámbito y resolución de nombres.
3. **Verificación de Validez**: Simplifica la validación de reglas semánticas específicas del dominio de redes.

## Conclusión

La implementación del AST proporciona una base robusta para el compilador del DSL de configuración Mikrotik, con un diseño que equilibra la seguridad de tipos, la mantenibilidad y los requisitos específicos del dominio. La estructura jerárquica representa con precisión la naturaleza anidada de las configuraciones de red, facilitando el análisis semántico y la generación de código ejecutable.

Este diseño permite a los administradores de red definir configuraciones complejas de manera declarativa, manteniendo la seguridad de tipo y las validaciones semánticas necesarias para garantizar configuraciones correctas y seguras. 