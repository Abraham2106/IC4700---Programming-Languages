# Operadores de Acceso, Índice y Llamada — Sobrecarga de [], -> y () para simular contenedores y functores

Sobrecarga de operadores de acceso (`->`), índice (`[]`) y llamada a función (`()`) permite crear abstracciones de punteros inteligentes, colecciones personalizadas y functores. Estas interfaces unifican la sintaxis de tipos de usuario con las construcciones nativas.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
- **`operator->` (Acceso a Miembro):** Se usa para que un objeto actúe como un puntero. Devuelve un puntero u otro objeto que implemente el operador `->` en cadena.
- **`operator[]` (Subíndice):** Habilita la sintaxis de arreglo en un objeto. Típicamente toma una clave o índice y devuelve una referencia al elemento correspondiente.
- **`operator()` (Llamada de Función):** Permite invocar a un objeto usando la sintaxis de una función tradicional. Los objetos de este tipo se denominan *functors* (objetos función).

### 1.2 ¿Por qué importa?
- Estos operadores son la base sintáctica para la abstracción de tipos complejos en la biblioteca estándar de C++:
  - `std::unique_ptr` usa `operator->` para simular punteros nativos.
  - `std::vector` y `std::map` usan `operator[]` para acceso de elementos.
  - Las expresiones lambda y algoritmos usan `operator()` para inyectar lógica ejecutable con estado persistente.

---

## 2. Hook Example

```cpp
#include <iostream>
#include <vector>

class ContenedorInvocable {
    std::vector<int> datos;
public:
    ContenedorInvocable() : datos{10, 20, 30, 40} {}

    // 1. operator[] miembro con versiones const y no-const
    int& operator[](size_t index) {
        return datos[index];
    }
    const int& operator[](size_t index) const {
        return datos[index];
    }

    // 2. operator() (Functor) para acumular valores
    int operator()(int multiplicador) const {
        int suma = 0;
        for (int v : datos) {
            suma += v * multiplicador;
        }
        return suma;
    }
};

// Clase auxiliar para ilustrar operator->
struct Item {
    void accion() const { std::cout << "Accion del Item ejecutada.\n"; }
};

class SmartPointerItem {
    Item* ptr;
public:
    explicit SmartPointerItem(Item* p) : ptr(p) {}
    ~SmartPointerItem() { delete ptr; }

    // 3. operator-> retorna el puntero nativo subyacente
    Item* operator->() const { return ptr; }
};

int main() {
    ContenedorInvocable c;
    c[1] = 99; // Llama a operator[] no-const
    std::cout << "Elemento 1: " << c[1] << "\n"; // Imprime 99

    // Llama a operator() (Functor)
    int resultado = c(2); // (10 + 99 + 30 + 40) * 2 = 179 * 2 = 358
    std::cout << "Resultado functor: " << resultado << "\n";

    // Llama a operator->
    SmartPointerItem sp(new Item());
    sp->accion(); // Sintaxis limpia equivalente a (sp.operator->())->accion()

    return 0;
}
```

---

## 3. Descomposición Under the Hood

### 3.1 La mecánica recursiva profunda de `operator->`
A diferencia de otros operadores binarios de C++ que toman un operando izquierdo y uno derecho en una sola operación, el operador `->` es un operador unario con una semántica de evaluación especial definida por el compilador:
1. Si la expresión es `objeto->miembro`:
2. Si `objeto` es un puntero nativo de C++ (`T*`), se realiza la dereferencia directa en la CPU y se accede al miembro de la dirección física.
3. Si `objeto` es una instancia de una clase, el compilador llama a `objeto.operator->()`.
4. El compilador toma el valor devuelto por `operator->()` y lo evalúa nuevamente aplicando el operador `->` sobre ese resultado de forma recursiva.
5. El proceso se detiene únicamente cuando se llega a un puntero primitivo de C++.

#### Diagrama de resolución de `->`:
```
   [p->Member()]  (p es un objeto de clase)
         │
         ▼
[Llamar p.operator->()] ───> Retorna objeto 'x'
         │
         ▼
[¿Es 'x' puntero nativo?] ──(No)──> [Llamar x.operator->()]
         │ (Sí)                         │
         ▼                              ▼
  [Acceso Nativo] <─────────────────────┘
```

---

## 4. Teoría: Reglas Específicas de operator->, operator[] y operator()

### 4.1 1. Obligación de ser Funciones Miembro
C++ prohíbe definir estos operadores como funciones no miembro (libres). Escribir `friend T* operator->(const MiClase&)` o similares producirá un error inmediato de compilación. Esto garantiza que el lado izquierdo de la expresión sea siempre un objeto legítimo de la clase contenedora.

### 4.2 2. Parámetros del operator[]
- Debe aceptar exactamente **un** argumento.
- Si requieres acceso multidimensional (ej. `matriz[x, y]`), C++ tradicional no lo permitía directamente con `[]` (requiere envolver las coordenadas en una estructura auxiliar, como se muestra en el libro *Secrets of C++ Masters*, o sobrecargar `operator()`).
- *Nota moderna:* Desde C++23 se permite pasar múltiples argumentos dentro de `operator[]`, pero la compatibilidad hacia atrás prefiere functors o structs auxiliares para representar índices compuestos.

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Acceso de Lectura/Escritura en Colecciones
El diseño estándar de `operator[]` siempre incluye dos sobrecargas:
- `T& operator[](size_t index)` para permitir asignaciones (`c[0] = valor`).
- `const T& operator[](size_t index) const` para su uso seguro en contextos constantes.

### 5.2 Nivel Aplicado: Arreglos Multidimensionales simulados con anónimos
El libro describe cómo usar clases auxiliares para simular múltiples dimensiones:
```cpp
struct Indice3D {
    int x, y, z;
    Indice3D(int px, int py, int pz) : x(px), y(py), z(pz) {}
};
// Uso: arreglo[Indice3D(1, 2, 3)]
```

### 5.3 Nivel Complejo: Functors para algoritmos de la STL
Los functors creados con `operator()` permiten mantener estado local y ser pasados a algoritmos como `std::for_each` o `std::sort`. Esto ofrece mejor rendimiento en compilación que los punteros a funciones tradicionales porque el compilador puede inlinear la llamada directamente.

---

## 6. Diseño de Sistemas
En el patrón de diseño Proxy y en la programación de envoltorios de comunicación, `operator->` es crucial para desviar llamadas de interfaz de manera transparente a objetos remotos o perezosos (lazy-loaded).

---

## Exercises

### Exercise 1 — Crear una matriz 2D con operator()
Implementa una clase `Matriz2D` de enteros con un tamaño fijo. Sobrecarga el `operator()` para permitir leer y escribir elementos usando la sintaxis `matriz(fila, columna)`.

---

## 7. Errores Comunes y Anti-Patrones
- **No proveer la sobrecarga `const` de `operator[]`:** Impide que los contenedores sean leídos si son pasados como referencia constante a funciones de renderizado o cálculo.
- **Retornar punteros locales en `operator->`:** Causa fugas de memoria o punteros inválidos si el objeto retornado es temporal y se destruye antes de resolver la llamada.

---

## 8. Conclusión

---

---

*Next: `29 — Sobrecarga de los operadores new y delete y placement new.md` — Alocadores personalizados y construcción en memoria prealocada.*
