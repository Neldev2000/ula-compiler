# Análisis Sintáctico para el DSL de Configuración Mikrotik

## Introducción

El analizador sintáctico (parser) constituye la segunda fase fundamental en el proceso de compilación de nuestro Lenguaje Específico de Dominio (DSL) orientado a la configuración de dispositivos Mikrotik. Este componente es responsable de analizar la secuencia de tokens generada por el analizador léxico y determinar si cumple con las reglas gramaticales del lenguaje. El parser transforma la secuencia lineal de tokens en una estructura jerárquica que refleja la organización lógica de la configuración de red.

## Objetivos del Diseño del Parser

- **Precisión Sintáctica**: Garantizar que las configuraciones sigan estrictamente las reglas definidas en la gramática del DSL.
- **Detección de Errores**: Proporcionar mensajes de error claros y específicos que faciliten la corrección de problemas sintácticos.
- **Representación Jerárquica**: Construir una estructura de datos que refleje la naturaleza anidada de las configuraciones de red.
- **Eficiencia Computacional**: Implementar un análisis sintáctico eficiente y escalable para procesar configuraciones complejas.
- **Integración Semántica**: Sentar las bases para el posterior análisis semántico y generación de código.

## Enfoque de Análisis Sintáctico

### Técnica LALR(1)

El parser de nuestro DSL está implementado utilizando la técnica LALR(1) (Look-Ahead LR), que ofrece un equilibrio óptimo entre expresividad y eficiencia:

- **Análisis Ascendente**: Procesa la entrada de izquierda a derecha, construyendo la estructura sintáctica desde los componentes más pequeños hasta los más grandes.
- **Sensibilidad al Contexto**: Utiliza un token de "anticipación" (look-ahead) para tomar decisiones sobre cómo interpretar la parte actual de la configuración.
- **Resolución de Ambigüedades**: Capaz de manejar y resolver ambigüedades comunes en la sintaxis de configuración de redes.

### Justificación de la Elección de LALR(1)

La elección de LALR(1) sobre otras técnicas de análisis sintáctico se fundamenta en consideraciones técnicas específicas:

#### Comparativa con Gramáticas LL (LL(0), LL(1))

- **LL(0)**: Los analizadores predictivos sin anticipación son demasiado restrictivos para nuestro DSL. No pueden manejar las ambigüedades que surgen naturalmente en la sintaxis de configuración de redes.

- **LL(1)**: Los analizadores descendentes con un token de anticipación requerirían transformaciones excesivas en nuestra gramática:
  - Eliminación de la recursión izquierda para expresiones como secciones anidadas
  - La factorización izquierda necesaria haría que la gramática fuera menos intuitiva y más difícil de mantener
  - Nuestro DSL contiene construcciones que son inherentemente más fáciles de analizar de manera ascendente que descendente

#### Comparativa con LR(1) Completo

- **LR(1)**: Aunque más potentes que LALR(1), los analizadores LR(1) presentan desventajas significativas:
  - Generan tablas de análisis mucho más grandes (a menudo exponencialmente mayores)
  - Requieren más memoria y pueden ser más lentos en ejecución
  - La potencia adicional de LR(1) no es necesaria para la gramática de nuestro DSL

#### Ventajas de LALR(1) para Nuestro DSL

- **Implementación Práctica**: El uso de Bison (que implementa LALR(1)) proporciona un generador de analizadores robusto y bien probado.
- **Análisis Eficiente**: Los analizadores LALR(1) tienen tablas de análisis compactas en comparación con LR(1).
- **Poder Suficiente**: LALR(1) puede manejar toda la gama de sintaxis de nuestro DSL sin limitaciones.
- **Manejo de Errores**: Proporciona mejores diagnósticos de error que los analizadores LL, permitiendo ofrecer a los usuarios comentarios claros sobre errores sintácticos.
- **Resolución de Ambigüedades**: Puede resolver ambigüedades comunes en la sintaxis de configuración de red con mínima complejidad gramatical.

## Estructura Gramatical del DSL

### Reglas Sintácticas Fundamentales

El DSL está diseñado con una sintaxis clara y concisa, enfocada en la legibilidad y facilidad de uso:

1. **Organización en Secciones**: La configuración se estructura en secciones temáticas (`device:`, `interfaces:`, etc.)
2. **Asignación de Propiedades**: Las propiedades se asignan mediante el signo igual (`vendor = "mikrotik"`)
3. **Estructura Jerárquica**: La jerarquía se crea utilizando dos puntos (`ether1:`)
4. **Ausencia de Punto y Coma**: No se permiten puntos y coma al final de las declaraciones
5. **Indentación para Legibilidad**: Se utiliza indentación para mejorar la legibilidad (aunque no es obligatoria para el parser)

### Ejemplos de Sintaxis Válida e Inválida

#### ✅ Sintaxis Válida:
```
device:
    vendor = "mikrotik"
    model = "CCR2004-1G-12S+2XS"
```

#### ❌ Sintaxis Inválida:
```
device:
    vendor = "mikrotik";  # Los puntos y coma no están permitidos
```

## Componentes Principales del DSL

El DSL admite seis tipos principales de secciones:

### 1. Sección `device`

Define la información básica del dispositivo:
```
device:
    vendor = "mikrotik"
    model = "CCR2004-1G-12S+2XS"
```

### 2. Sección `interfaces`

Configura las interfaces de red:
```
interfaces:
    ether1:
        type = "ethernet"
        admin_state = "enabled"
        description = "WAN Connection"
```

### 3. Sección `ip`

Define direccionamiento IP y servicios relacionados:
```
ip:
    address:
        ether1 = 192.168.1.1/24
    dhcp:
        server:
            enabled = true
```

### 4. Sección `routing`

Configura protocolos de enrutamiento y rutas estáticas:
```
routing:
    static:
        default_gateway = 192.168.1.254
```

### 5. Sección `firewall`

Define reglas de firewall y NAT:
```
firewall:
    filter:
        input_accept_established:
            chain = "input"
            connection_state = ["established", "related"]
            action = "accept"
```

### 6. Sección `system`

Configuración a nivel del sistema:
```
system:
    hostname = "core-router"
    timezone = "America/Caracas"
```

## Estructuras Sintácticas Fundamentales

### Secciones

Las secciones son los bloques de construcción de nivel superior de la configuración:

```
device:
    # propiedades y subsecciones aquí
```

### Propiedades

Las propiedades son pares clave-valor que definen atributos de configuración:

```
vendor = "mikrotik"
model = "CCR2004"
enabled = true
port = 123
address = 192.168.1.1/24
```

### Bloques Anidados

Los bloques pueden anidarse para crear configuraciones jerárquicas:

```
interfaces:
    ether1:
        type = "ethernet"
        ethernet:
            speed = "1Gbps"
```

### Listas

Los valores también pueden ser listas, encerradas entre corchetes:

```
firewall:
    filter:
        connection_state = ["established", "related"]
```

## Errores Sintácticos Comunes

Nuestro parser proporciona mensajes de error claros para problemas sintácticos comunes:

1. **Uso de punto y coma**: No se permiten puntos y coma en este DSL.
   ```
   vendor = "mikrotik";  # ERROR: No se permiten puntos y coma en este DSL
   ```

2. **Tokens inválidos después de dos puntos**: Después de una declaración de sección, solo se permite un salto de línea o un comentario.
   ```
   interfaces: ;  # ERROR: Sintaxis inválida después de dos puntos
   ```

3. **Asignación de propiedad inválida**: 
   ```
   vendor $ "mikrotik"  # ERROR: Sintaxis de asignación de propiedad inválida
   ```

4. **Tokens desconocidos o sintaxis inválida**:
   ```
   device: @  # ERROR: Token desconocido o sintaxis inválida
   ```

## Manejo de Comentarios

El DSL admite dos tipos de comentarios:

1. **Comentarios de una sola línea** que comienzan con `#`:
   ```
   # Esto es un comentario
   device:  # Esto es un comentario al final de la línea
   ```

2. **Comentarios multilínea** encerrados en comillas triples `"""`:
   ```
   """
   Este es un comentario multilínea
   que abarca varias líneas
   """
   ```

## Representación Orientada a Objetos

La implementación del parser utiliza un enfoque orientado a objetos para representar y manipular configuraciones de red a través de los archivos `expressions.hpp` y `expressions.cpp`.

### Jerarquía de Clases de Configuración

La implementación sigue una estructura de clases jerárquica con `ConfigNode` como clase base:

#### Clase Base: ConfigNode

```cpp
class ConfigNode {
public:
    virtual ~ConfigNode();
    virtual void destroy() noexcept = 0;
    virtual std::string to_string() const noexcept = 0;
};
```

Esta clase base abstracta define la interfaz para todos los componentes de configuración, garantizando:
- **Limpieza de Recursos**: A través del método `destroy()`
- **Representación Textual**: Mediante el método `to_string()` para depuración y serialización

#### Jerarquía de Clases

- **Value**:
  - Representa valores primitivos en la configuración (cadenas, números, booleanos, direcciones IP)
  - Cada valor tiene tanto un contenido como un tipo (enumeración ValueType)
  - Los tipos incluyen STRING, NUMBER, BOOLEAN, IP_ADDRESS, IP_CIDR, y otros

- **ListValue**:
  - Representa listas de valores, como múltiples direcciones IP o reglas de firewall
  - Mantiene y gestiona un vector de objetos Value
  - Proporciona iteración y acceso a los valores contenidos

- **Property**:
  - Representa una propiedad de configuración con un nombre y un valor
  - Ejemplo: `vendor = "mikrotik"` o `admin_state = "enabled"`
  - El valor puede ser un Value simple o un ListValue más complejo

- **Block**:
  - Representa una colección de declaraciones de configuración (propiedades o subsecciones)
  - Mantiene un vector de declaraciones y proporciona iteración sobre ellas
  - Se utiliza para agrupar elementos de configuración relacionados

- **Section**:
  - Representa una sección nombrada en la configuración con un tipo específico
  - Los tipos de sección se definen en la enumeración SectionType (DEVICE, INTERFACES, IP, etc.)
  - Contiene un Block de declaraciones para el contenido de la sección

- **Configuration**:
  - Clase de nivel superior que representa la configuración de red completa
  - Contiene una colección de objetos Section
  - Sirve como punto de entrada para la configuración analizada

### Ventajas de este Diseño de Clases

Este enfoque basado en clases ofrece varios beneficios:

1. **Representación Jerárquica**: Refleja la estructura natural de las configuraciones de red
2. **Seguridad de Tipos**: Cada elemento de configuración tiene un tipo específico con operaciones apropiadas
3. **Gestión de Memoria**: La limpieza estructurada a través de métodos destroy() virtuales previene fugas de memoria
4. **Soporte de Validación**: Permite la validación específica por tipo de los valores de configuración
5. **Serialización**: Fácil conversión a representación de cadena mediante métodos to_string()
6. **Recorrido**: Recorrido simple del árbol de configuración para análisis o transformación

### Integración con el Parser LALR(1)

El parser Bison (`parser.bison`) construye esta jerarquía de objetos a medida que reconoce patrones gramaticales:

1. Cuando se reconocen tokens, se instancian objetos apropiados (Value, Property, etc.)
2. Estos objetos se combinan en estructuras más complejas según las reglas gramaticales
3. Las secciones se pueblan con propiedades y secciones anidadas
4. El resultado final es un objeto Configuration completo que representa la configuración de red analizada
5. El parser almacena este resultado en la variable `parser_result` para su posterior procesamiento

## Conclusión

El analizador sintáctico está diseñado para validar y procesar configuraciones Mikrotik escritas en nuestro DSL. Impone una sintaxis limpia y legible sin puntuación innecesaria como puntos y coma, centrándose en una estructura jerárquica que refleja la organización lógica de las configuraciones de red.

Siguiendo las reglas sintácticas descritas en este documento, los administradores pueden crear configuraciones válidas que serán aceptadas por el parser y eventualmente traducidas a comandos compatibles con Mikrotik. Esta aproximación permite una configuración de red más intuitiva y menos propensa a errores, mejorando significativamente la experiencia del usuario y la confiabilidad de las implementaciones de red. 