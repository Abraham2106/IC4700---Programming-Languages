# 21 — Sobrecarga de Operadores (operator)

> **Resumen Ejecutivo:** La sobrecarga de operadores en C++ permite redefinir el comportamiento de los operadores nativos (`+`, `-`, `*`, `<<`, `[]`, etc.) sobre clases y estructuras personalizadas. Esto facilita la integración de nuevos tipos dentro de la sintaxis estándar del lenguaje de forma intuitiva.
>
> **Prerrequisitos:** Haber leído [11 — Sobrecarga (Overloading)](11 — Sobrecarga (Overloading).md).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Reglas de Sobrecarga de Operadores](#teoría-reglas-de-sobrecarga-de-operadores)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
La sobrecarga de operadores consiste en definir funciones especiales con la palabra clave `operator` seguida del símbolo a sobrecargar (ej. `operator+`), indicando al compilador cómo procesar operaciones algebraicas o lógicas sobre nuestros objetos.

### ¿Por qué importa?
Habilita la legibilidad matemática y de flujos. En lugar de escribir llamadas engorrosas como `v1.sumar(v2).multiplicar(5)`, podemos escribir directamente `(v1 + v2) * 5`, integrando el comportamiento de forma natural.

---

## Conceptos Previos
- Concepto de funciones libres (no miembros) vs métodos de clase.
- Qué es el paso de parámetros por referencia constante.

---

## Hook Example

```cpp
#include <iostream>

struct Vector2D {
    float x, y;

    // Sobrecarga como función miembro (operador binario +)
    Vector2D operator+(const Vector2D& otro) const {
        return Vector2D{x + otro.x, y + otro.y};
    }
};

// Sobrecarga de flujo de salida como función no miembro
std::ostream& operator<<(std::ostream& os, const Vector2D& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os; // Permite encadenamiento
}

int main() {
    Vector2D v1{1.0f, 2.0f};
    Vector2D v2{3.0f, 4.0f};
    Vector2D v3 = v1 + v2; // Equivalente a v1.operator+(v2)
    
    std::cout << "Resultado: " << v3 << "\n";
    return 0;
}
```

---

## Descomposición Under the Hood

### ¿Qué hace el compilador con un operador sobrecargado?
- La sobrecarga de operadores es enteramente **azúcar sintáctica** en tiempo de compilación.
- Cuando el compilador parsea la línea `v1 + v2`:
  1. Busca si existe un método dentro de la clase de `v1` con la firma `v1.operator+(v2)`.
  2. Si no lo encuentra, busca en el espacio de nombres global o local una función libre con firma `operator+(v1, v2)`.
  3. Si existe, la llamada se compila idénticamente a una función ordinaria. No existe overhead en tiempo de ejecución (runtime overhead) por usar la sintaxis de operadores en lugar de funciones tradicionales.

---

## Teoría: Reglas de Sobrecarga de Operadores

### 1. Limitaciones Clave
- **No se pueden crear operadores nuevos:** No puedes inventar operadores como `**` o `%%`. Solo puedes sobrecargar operadores existentes en el estándar de C++.
- **La precedencia y asociatividad no cambian:** El operador `*` siempre tendrá mayor prioridad de evaluación que el operador `+`.
- **Al menos un argumento debe ser un tipo definido por el usuario:** No puedes redefinir el comportamiento de `1 + 2` sobre enteros primitivos.
- Operadores que **deben** ser funciones miembro obligatoriamente: `operator=` (asignación), `operator[]` (índice), `operator->` (acceso a miembros), `operator()` (llamada a función).

### 2. Miembro vs No Miembro
- **Función Miembro:** El argumento de la izquierda del operador es implícitamente `this`.
- **Función No Miembro:** Útil para operadores simétricos o cuando la clase de la izquierda no es modificable (ej. `std::ostream` en `std::cout << objeto`). Suele requerir declararse como `friend` para acceder a atributos privados.

---

## Progresión de Complejidad

### Nivel Simple: Operador de Comparación (`operator==`)
```cpp
struct Punto {
    int x, y;
    bool operator==(const Punto& otro) const {
        return x == otro.x && y == otro.y;
    }
};
```

### Nivel Aplicado: Operador de Índice (`operator[]`)
Permite acceder y modificar elementos como si el objeto fuera un arreglo nativo.
```cpp
#include <vector>

class Matriz {
    std::vector<int> datos;
public:
    Matriz(int n) : datos(n) {}
    // Permite lectura y escritura: m[0] = 5
    int& operator[](size_t idx) { return datos[idx]; }
    // Permite lectura en objetos constantes
    const int& operator[](size_t idx) const { return datos[idx]; }
};
```

### Nivel Complejo: Sobrecarga del Operador de Llamada (`operator()`) - Functors
Habilita que un objeto se comporte como una función (functor), pudiendo mantener estado interno persistente.
```cpp
#include <iostream>

class SumadorAcumulado {
    int total = 0;
public:
    int operator()(int valor) {
        total += valor;
        return total;
    }
};

int main() {
    SumadorAcumulado suma;
    std::cout << suma(5) << "\n"; // Imprime 5
    std::cout << suma(10) << "\n"; // Imprime 15
    return 0;
}
```

---

## Diseño de Sistemas
En C++ moderno (desde C++20), se prefiere sobrecargar únicamente el operador de comparación de tres vías (`operator<=>`), comúnmente llamado operador nave espacial (spaceship operator). A partir de esta única función, el compilador deduce automáticamente todos los operadores relacionales asociados (`<`, `<=`, `>`, `>=`, `==`, `!=`).

---

## Ejercicios

### Ejercicio 1 — Sobrecargar Operador de Multiplicación por Escalar
Sobrecarga el operador de multiplicación para permitir escalar un `Vector` por un valor de tipo `float` por la derecha.

```cpp
#include <iostream>

struct Vector {
    float x, y;
    
    // TODO: Sobrecargar el operator* para multiplicar x e y por un factor float
    // Vector operator*(float factor) const;
};

int main() {
    Vector v{2.0f, 3.0f};
    
    // Vector res = v * 2.0f; // Debería dar (4.0, 6.0)
    // std::cout << "Resultado: " << res.x << ", " << res.y << "\n";
    return 0;
}
```

---

## Errores Comunes y Anti-Patrones
- **Violar la semántica esperada:** Sobrecargar `operator+` para que reste en lugar de sumar. Confunde gravemente a otros desarrolladores y arruina la mantenibilidad.
- **Retornar referencias a objetos locales:** En operadores aritméticos como `+`, retornar referencias (`T&`) en lugar de copias por valor (`T`) causa dangling references inmediatas, ya que el objeto temporal resultante de la suma se destruye al salir de la función del operador.

---

## Conclusión y Checklist Mental
- [ ] ¿Cuáles operadores están prohibidos para sobrecarga en C++?
- [ ] ¿Por qué el operador de salida `<<` se suele sobrecargar como función libre en lugar de método miembro?
- [ ] ¿Cómo funciona el operador de comparación espacial `<=>` introducido en C++20?

---

*Siguiente tema sugerido: [22 — Polimorfismo y Funciones Virtuales](<22 — Polimorfismo y Funciones Virtuales.md>)*


