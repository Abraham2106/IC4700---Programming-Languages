# 27 — Operadores No Miembro y Conversión

> **Resumen Ejecutivo:** Los operadores en C++ se pueden sobrecargar como funciones miembro o como funciones libres (no miembros). Las funciones no miembro son cruciales cuando el operando izquierdo no es una clase bajo nuestro control o se desea simetría en conversiones implícitas. Asimismo, los operadores de conversión (`operator T()`) permiten mapear un tipo de usuario a otro tipo, requiriendo el uso defensivo de la palabra clave `explicit` (C++11) para evitar conversiones implícitas destructivas en tiempo de compilación.
>
> **Prerrequisitos:** Haber leído [11 — Sobrecarga (Overloading)](<11 — Sobrecarga (Overloading).md>) y [21 — Sobrecarga de Operadores (operator)](<21 — Sobrecarga de Operadores (operator).md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Miembro vs. No Miembro y Operadores de Conversión](#teoría-miembro-vs-no-miembro-y-operadores-de-conversión)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
- **Non-Member Overloads:** Son operadores definidos fuera del ámbito de la clase (funciones libres) que toman todos los parámetros de manera explícita. Frecuentemente se declaran como `friend` dentro de la clase para acceder a la representación privada de los datos.
- **Conversion Operators:** Son funciones miembro que permiten que un objeto sea implícita o explícitamente promovido a otro tipo de datos (por ejemplo, convertir un objeto `String` a un puntero `const char*` o a un `long`).

### ¿Por qué importa?
- **Simetría:** Si el operador es una función miembro, el operando izquierdo debe ser estrictamente un objeto de esa clase. Una expresión como `2.0 + Vector` fallará si el operador `+` solo es miembro de la clase `Vector`. Definirlo como no miembro permite que tanto el lado izquierdo como el derecho admitan conversiones.
- **Seguridad en Conversiones:** Sin la directiva `explicit` en un operador como `operator bool()`, el compilador podría promover silenciosamente el objeto a un entero y participar en operaciones matemáticas sin generar errores en tiempo de compilación.

---

## Conceptos Previos
- Palabras clave `friend` y `explicit`.
- Orden de búsqueda de sobrecargas de operadores del compilador.

---

## Hook Example

```cpp
#include <iostream>
#include <cstdlib>

class EnteroSeguro {
    int valor;
public:
    explicit EnteroSeguro(int v) : valor(v) {}

    // 1. Operador de conversion explicit (C++11)
    // Previene conversiones implicitas accidentales en expresiones matematicas
    explicit operator int() const {
        return valor;
    }

    // 2. Operador no miembro simetrico declarado como friend para acceder a 'valor'
    friend EnteroSeguro operator+(const EnteroSeguro& lhs, const EnteroSeguro& rhs);
};

// Definicion del operador libre (no miembro)
EnteroSeguro operator+(const EnteroSeguro& lhs, const EnteroSeguro& rhs) {
    return EnteroSeguro(lhs.valor + rhs.valor);
}

int main() {
    EnteroSeguro e1(10);
    EnteroSeguro e2(20);

    EnteroSeguro e3 = e1 + e2; // Llama a operator+ no miembro

    // int raw = e3; // ERROR: La conversion es explicita
    int raw = static_cast<int>(e3); // Correcto: conversion explicita via static_cast
    std::cout << "Valor raw: " << raw << "\n";

    return 0;
}
```

---

## Descomposición Under the Hood

### ¿Cómo encuentra el compilador los operadores no miembros? (ADL - Argument Dependent Lookup)
Cuando escribes `x + y`, el compilador realiza una búsqueda de sobrecargas que incluye la búsqueda en el espacio de nombres de los tipos de los argumentos. Este mecanismo se conoce como **ADL (o búsqueda de Koenig)**:
1. Si `x` es de tipo `MiEspacio::ClaseA`, el compilador busca funciones `operator+` no solo en el espacio de nombres global, sino también dentro del espacio de nombres `MiEspacio` aunque la llamada no esté calificada.
2. Esto permite que los operadores libres definidos junto a sus clases correspondientes sean encontrados automáticamente sin ensuciar el espacio de nombres global con miles de declaraciones visibles.

---

## Teoría: Miembro vs. No Miembro y Operadores de Conversión

### 1. Cuándo preferir funciones no miembro
Debes preferir funciones libres (no miembros) para operadores simétricos y de flujo:
- **Operadores Relacionales y Aritméticos:** Como `+`, `-`, `==`, `<`, cuando deseas que conversiones implícitas apliquen por igual en el operando de la izquierda y de la derecha (por ejemplo, `Entero + 5` y `5 + Entero`).
- **Operador de Inserción/Extracción de Flujo (`<<` y `>>`):** Debido a que el primer parámetro es de tipo `std::ostream&` o `std::istream&` (clases de la biblioteca estándar que no podemos modificar), estos operadores **deben** ser funciones no miembro.

### 2. Sintaxis de los Operadores de Conversión
- No tienen tipo de retorno explícito en su declaración (el tipo de retorno se deduce del nombre del operador).
- No toman argumentos.
- Firma genérica: `operator TipoDestino() const;`

---

## Progresión de Complejidad

### Nivel Simple: Conversión Implícita Antigua
En versiones previas a C++11, no se podía marcar un operador de conversión como `explicit`. Clases que implementaban `operator bool()` para verificar su estado (como streams o punteros inteligentes) sufrían el peligro de ser convertidas automáticamente a `int` por el compilador, permitiendo código absurdo como `archivo_stream + 5` sin generar error de compilación.

### Nivel Aplicado: Firma const y no-const de operadores de conversión
Como resalta *Secrets of C++ Masters*, es común proveer ambas variantes. La versión `const` suele devolver un tipo por valor o referencia constante (muy rápida), mientras que la versión no-const puede requerir hacer copias de seguridad de datos internos para preservar el encapsulamiento del objeto original si el cliente pretende modificar la estructura de datos retornada.

### Nivel Complejo: Comparador Espacial (Spaceship Operator `<=>` de C++20)
En C++20, en lugar de escribir 6 sobrecargas diferentes para comparar objetos (`==`, `!=`, `<`, `<=`, `>`, `>=`), se implementa una única función no miembro de comparación de tres vías (`operator<=>`).
- El compilador deduce y autogenera todas las demás operaciones lógicas a partir de esta.
- Esto elimina toneladas de código redundante e incrementa la legibilidad drásticamente.

---

## Diseño de Sistemas
En el diseño de bibliotecas modernas de C++, se favorece el uso de constructores marcados con `explicit` y operadores de conversión de igual manera marcados como `explicit`. Esto obliga a los consumidores de la API a documentar sus intenciones mediante `static_cast`, reduciendo significativamente la posibilidad de bugs lógicos silenciosos.

---

## Ejercicios

### Ejercicio 1 — Implementar el operador de flujo `<<`
Crea una clase `Punto2D` con atributos privados `x` e `y`. Sobrecarga el operador no miembro `<<` para que permita imprimir el punto en consola con el formato `(x, y)` usando `std::cout << punto;`.

---

## Errores Comunes y Anti-Patrones
- **Ambigüedad Bidireccional:** Proveer un constructor de conversión de `A` a `B` (`B::B(const A&)`) y al mismo tiempo proveer un operador de conversión de `A` a `B` (`A::operator B()`). El compilador detendrá la compilación por ambigüedad al intentar convertir de `A` a `B`.
- **Modificar el objeto origen dentro de un operador de conversión no-const sin documentarlo:** Confunde a los desarrolladores que esperan que un cast actúe como una operación de lectura libre de efectos colaterales.

---

## Conclusión y Checklist Mental
- [ ] ¿Por qué el operador de inserción en flujo `<<` debe declararse obligatoriamente como una función no miembro?
- [ ] ¿Qué problema previene el uso de la palabra clave `explicit` en un operador de conversión?
- [ ] ¿Cómo ayuda la búsqueda ADL a simplificar el enlazado de operadores en namespaces?
- [ ] ¿Cómo funciona el operador `<=>` introducido en C++20?

---

*Siguiente tema sugerido: [28 — Operador accesor, operador de indice y operador de funcion](<28 — Operador accesor, operador de indice y operador de funcion.md>)*
