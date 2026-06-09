# Idiosincrasias del C++ — Tipos, Modificadores y Uniones

El diseño de C++ requiere un control absoluto sobre el tipado estático y el uso de la memoria física. El lenguaje ofrece mecanismos rigurosos para garantizar inmutabilidad, optimizar almacenamiento y aislar ámbitos, dejando atrás las prácticas inseguras heredadas de C.

---

## 1. Modificadores de Inmutabilidad y Ámbito

La inmutabilidad en C++ es un contrato que el compilador verifica estrictamente para prevenir efectos colaterales. A diferencia de C, donde abundaban las macros globales, C++ exige que la constante se defina con tipo y ámbito físico verificable.

### 1.1 Constantes vs Preprocesador (`const` vs `#define`)

El uso de la directiva `#define` para constantes delega el trabajo al preprocesador, el cual realiza una sustitución textual antes de que el compilador construya el Árbol de Sintaxis Abstracta (AST). Esto significa que las macros carecen de tipo, no respetan reglas de ámbito (namespace o clase) y no existen en la tabla de símbolos para el depurador.

Para garantizar seguridad de tipos (type-safety), C++ exige el uso de `const` (o `constexpr`). Una variable `const` tiene un tipo explícito, ocupa una dirección física si se toma su referencia, y respeta las jerarquías de encapsulamiento estricto.

### 1.2 La Regla "Una vez constante, para siempre constante"

Cuando un parámetro se pasa a una función como una referencia o puntero constante, C++ sella esa inmutabilidad de forma transitiva. Cualquier intento de violar este contrato, por ejemplo, pasando un objeto `const` a una función que recibe un argumento no constante, resulta en un error de compilación irrecuperable.

```cpp
void imprimir(const std::string& texto) {
    // texto = "modificado"; // ERROR: El compilador rechaza la mutación.
}
```

Esta regla protege el estado de los objetos complejos garantizando que las funciones que prometen ser de sólo lectura no alteren la memoria bajo ninguna circunstancia.

### 1.3 Constantes Locales y Globales (enum y ::)

Para agrupar literales numéricos relacionados, el uso de enumeradores (`enum` o preferiblemente `enum class` moderno) agrupa constantes garantizando seguridad de tipos y encapsulamiento. 

El operador de resolución de ámbito (`::`) se utiliza para invocar estas constantes explícitas o para desambiguar llamadas globales frente a funciones locales que posean la misma firma, evitando colisiones de nombres o enmascaramiento accidental.

---

## 2. Uniones de Memoria (Unions)

Las uniones son tipos de datos donde todos los miembros se superponen y comparten la misma dirección física de memoria. El tamaño total de la unión será igual al tamaño en bytes de su miembro más grande. 

### 2.1 Restricciones Estructurales de las Uniones

Dado que C++ no puede garantizar qué campo de la unión está activo en un momento dado, impone restricciones fundamentales sobre su estructura para mantener la sanidad del ciclo de vida de los objetos subyacentes:

- **Herencia Prohibida:** Una unión no puede derivar de ninguna clase o estructura (no tiene clases base) y ninguna clase puede derivar de una unión.
- **Sin Polimorfismo Físico:** No existe el concepto de funciones virtuales puras, herencia virtual ni modificadores de acceso protegidos (`protected`) para uniones.
- **Ciclo de Vida Anómalo:** Los miembros internos de la unión no pueden invocar sus propios constructores ni destructores automáticamente. Sin embargo, la unión en sí misma *sí* puede tener un constructor general y un destructor general explícito.

```cpp
union VarianteFisica {
    int id_numerico;
    float proporcion;
    // std::string texto; // Problemático en C++ pre-C++11 debido al constructor complejo
};
```

### 2.2 Reemplazo Moderno: std::variant

El acceso al campo incorrecto de una unión tradicional produce una reinterpretación de bits (Type Punning), lo que causa comportamiento indefinido. 

C++ moderno introdujo `std::variant` como un tipo suma (tagged union) seguro y estático (type-safe). A diferencia de la unión desnuda, el variant asigna silenciosamente un byte interno (un "tag" o etiqueta) para llevar registro de qué tipo está activo actualmente, permitiendo comprobaciones dinámicas seguras y el manejo correcto del ciclo de vida (llamadas a constructores y destructores) de sus tipos contenidos.
