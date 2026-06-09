# Uniones y std::variant — Seguridad de tipos suma y optimización de espacio en memoria

Las uniones tradicionales de C permiten superponer múltiples variables en el mismo espacio físico de memoria, arriesgando la seguridad de tipos. `std::variant` proporciona un tipo suma seguro y moderno que realiza un seguimiento del tipo activo sin incurrir en alocación dinámica.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
- **Unión (`union`):** Estructura donde todos sus miembros comparten la misma dirección inicial de memoria. El tamaño de la unión es igual al tamaño de su miembro más grande.
- **Unión Etiquetada (`std::variant` - C++17):** Contenedor type-safe que almacena una de las alternativas de tipo declaradas, sabiendo en todo momento qué tipo está activo.

### 1.2 ¿Por qué importa?
Las uniones tradicionales carecen de información de tipo en runtime. Leer un miembro de tipo `float` cuando escribiste previamente en el miembro `int` produce una reinterpretación directa de bits (Type Punning), lo cual suele ser fuente de errores sutiles o comportamiento indefinido.

---

## 2. Hook Example

```cpp
#include <iostream>
#include <variant>

// Union clásica insegura
union DatosInseguros {
    int entero;
    float flotante;
};

int main() {
    DatosInseguros u;
    u.entero = 42;
    // Leer el flotante ahora es Type Punning y produce basura o comportamiento indefinido.
    std::cout << "Union flotante: " << u.flotante << "\n"; 

    // Variant C++17 seguro
    std::variant<int, float> v;
    v = 42;
    
    // std::get<float>(v); // Lanza un std::bad_variant_access en runtime de forma segura.
    std::cout << "Variant int: " << std::get<int>(v) << "\n";
    return 0;
}
```

---

## 3. Descomposición Under the Hood

### 3.1 Disposición de Memoria: Union vs Struct vs Variant
- Si declaras un struct con `int a` y `double b`, se reservan `sizeof(int) + padding + sizeof(double)` (usualmente 16 bytes). Sus miembros conviven secuencialmente.
- Si declaras una unión con esos mismos campos, se reservan únicamente `sizeof(double)` (8 bytes). La dirección de memoria base de `a` y `b` es exactamente la misma (`&u.a == &u.b`).
- `std::variant<int, double>` reserva suficiente espacio para albergar el tipo más grande (`double` -> 8 bytes) más un byte adicional (el **tag** o etiqueta) para llevar registro de cuál es el índice del tipo activo en ese instante en memoria RAM.

---

## 4. Teoría: Uniones Clásicas vs Variant Moderno

### 4.1 1. Uniones de C
- No pueden contener tipos con constructores o destructores no triviales (como `std::string` o `std::vector`) de forma sencilla, ya que el compilador no sabría cuándo llamar al destructor correcto de la memoria compartida.
- Carecen de validación sintáctica o en runtime del miembro activo.

### 4.2 2. `std::variant` (C++17)
- Permite albergar cualquier clase compleja de C++.
- Garantiza la llamada al destructor apropiado del tipo activo cuando el variant sale de ámbito.
- Provee la función `std::holds_alternative<T>(v)` para verificar si cierto tipo está activo.
- Facilita el procesamiento seguro mediante el patrón Visitor (`std::visit`).

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Unión Anónima
Se utiliza dentro de estructuras para agrupar campos sin requerir un nombre intermedio de acceso.
```cpp
struct Nodo {
    int tipo;
    union { // Union anónima
        int id_numerico;
        char codigo[4];
    };
};
```

### 5.2 Nivel Aplicado: Inspección de Tipos con `std::holds_alternative`
```cpp
#include <iostream>
#include <variant>

void procesar(const std::variant<int, std::string>& v) {
    if (std::holds_alternative<int>(v)) {
        std::cout << "Entero: " << std::get<int>(v) << "\n";
    } else {
        std::cout << "String: " << std::get<std::string>(v) << "\n";
    }
}
```

### 5.3 Nivel Complejo: Despacho Polimórfico Seguro usando `std::visit`
El uso de `std::visit` con expresiones lambda sobrecargadas permite realizar operaciones limpias basadas en el tipo activo del variant sin recurrir a herencia virtual clásica.
```cpp
#include <iostream>
#include <variant>

struct DibujarCirculo { void operator()(int r) const { std::cout << "Circulo r=" << r << "\n"; } };
struct DibujarRectangulo { void operator()(double l) const { std::cout << "Cuadrado l=" << l << "\n"; } };

int main() {
    std::variant<int, double> figura = 5.5;
    
    // Visitante polimórfico
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            std::cout << "Dibuja circulo de radio " << arg << "\n";
        } else if constexpr (std::is_same_v<T, double>) {
            std::cout << "Dibuja rectangulo de lado " << arg << "\n";
        }
    }, figura);
    
    return 0;
}
```

---

## 6. Diseño de Sistemas
En analizadores sintácticos (parsers) o compiladores, `std::variant` es ideal para modelar el Árbol de Sintaxis Abstracta (AST), donde cada nodo del árbol puede ser uno de múltiples tipos de tokens diferentes.

---

## Exercises

### Exercise 1 — Implementar una Unión Etiquetada Manual
Implementa una simulación rústica de unión etiquetada combinando un `struct`, un `enum` para la etiqueta y una `union` clásica para contener los datos.

```cpp
#include <iostream>

enum TipoDato { ENTERO, CARACTER };

struct UnionEtiquetada {
    TipoDato tag;
    union {
        int valor_int;
        char valor_char;
    } datos;
};

// TODO: Crear una función que imprima el dato correcto basándose en el 'tag'
void imprimir_dato(const UnionEtiquetada& u) {
    // Implementar
}

int main() {
    UnionEtiquetada u1;
    u1.tag = ENTERO;
    u1.datos.valor_int = 100;
    
    imprimir_dato(u1); // Debe imprimir el entero
    return 0;
}
```

---

## 7. Errores Comunes y Anti-Patrones
- **Uso accidental de un miembro inactivo en una unión:** Causa corrupción lógica de datos sin disparar advertencias del compilador.
- **No capturar excepciones al usar `std::get`:** Si no se está seguro de qué tipo reside en el `variant`, se debe capturar `std::bad_variant_access` o preferir el uso de `std::get_if`, el cual devuelve un puntero nulo (`nullptr`) si la alternativa solicitada no está activa en lugar de lanzar una excepción.

---

## 8. Conclusión

---

---

*Next: `14 — Herencia Privada y Composición.md` — Reutilización de código frente a relaciones jerárquicas.*
