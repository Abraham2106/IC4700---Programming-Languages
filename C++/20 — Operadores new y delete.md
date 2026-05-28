# 20 — Operadores new y delete

> **Resumen Ejecutivo:** `new` y `delete` son los operadores nativos de C++ para gestionar memoria dinámica en el Heap. A diferencia de `malloc` y `free` de C, estos operadores no solo alocan bytes crudos sino que invocan constructores y destructores de forma automática, garantizando la correctitud de los objetos de C++.
>
> **Prerrequisitos:** Haber leído [19 — Gestión de Memoria Stack vs Heap](<19 — Operadores new y delete.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Alocación Dinámica C vs C++](#teoría-alocación-dinámica-c-vs-c)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
- **`new`:** Operador que reserva memoria física en el Heap e inicializa el objeto llamando a su constructor.
- **`delete`:** Operador que invoca al destructor del objeto y devuelve la memoria física al asignador.

### ¿Por qué importa?
- **Constructor Trigger:** Usar funciones de C como `malloc` en C++ reserva la memoria física pero deja los miembros internos sin inicializar (no llama al constructor). Esto provoca un comportamiento indefinido (UB) inmediato al intentar leer miembros complejos como `std::string`.
- **Memory leaks:** La memoria del Heap no se autoliberará jamás al salir de ámbito. Es imperativo que cada llamada a `new` tenga su correspondiente llamada a `delete` (o usar Smart Pointers).

---

## Conceptos Previos
- Concepto de constructores y destructores.
- Diferencias operacionales entre Stack y Heap.

---

## Hook Example

```cpp
#include <iostream>
#include <string>

class Usuario {
    std::string nombre;
public:
    Usuario(std::string n) : nombre(n) { std::cout << "Usuario " << nombre << " creado.\n"; }
    ~Usuario() { std::cout << "Usuario " << nombre << " destruido.\n"; }
};

int main() {
    // C++: new aloca memoria E invoca al constructor
    Usuario* u1 = new Usuario("Arthas");
    
    // delete llama al destructor Y libera memoria
    delete u1; 
    
    // C clásico (PELIGROSO en C++): malloc no invoca constructores
    // Usuario* u2 = (Usuario*)malloc(sizeof(Usuario)); // Inicialización incompleta. Prohibido en C++.
    // free(u2);
    
    return 0;
}
```

---

## Descomposición Under the Hood

### ¿Qué traduce el compilador al ver `new` y `delete`?
- La llamada `Usuario* u = new Usuario("Arthas");` se expande internamente a:
  1. **Llamada de Alocación:** `void* memoria = operator new(sizeof(Usuario));` (reserva de memoria física).
  2. **Llamada de Construcción:** Ejecuta el constructor de `Usuario` sobre la dirección apuntada por `memoria` usando una instrucción especial llamada *Placement New*.
- La llamada `delete u;` se expande a:
  1. **Llamada de Destrucción:** Llama explícitamente al destructor `u->~Usuario();`.
  2. **Llamada de Liberación:** `operator delete(u);` (retorna los bytes al allocator).

---

## Teoría: Alocación Dinámica C vs C++

### Comparativa:
| Característica | `malloc` / `free` | `new` / `delete` |
|---|---|---|
| **Tipo de entidad** | Funciones de biblioteca (`<stdlib.h>`) | Palabras clave / Operadores nativos de C++ |
| **Invocación de Constructores** | No | Sí (Automática) |
| **Type Safety** | Retorna `void*` (Requiere cast explícito) | Retorna tipo tipado seguro directamente |
| **Cálculo de tamaño** | Manual via `sizeof(T)` | Automático por el compilador |
| **Manejo de fallos** | Retorna `nullptr` | Lanza excepción `std::bad_alloc` por defecto |

---

## Progresión de Complejidad

### Nivel Simple: Alocación de Arreglos (`new[]` y `delete[]`)
Si alocas un arreglo dinámico usando corchetes, debes usar obligatoriamente la versión de liberación con corchetes.
```cpp
int* arr = new int[50];
delete[] arr; // Correcto. delete arr; causa comportamiento indefinido.
```

### Nivel Aplicado: Placement New
Permite construir un objeto de C++ sobre una dirección de memoria física preexistente (por ejemplo, memoria alocada estáticamente o un buffer prealocado en Stack), evitando alocaciones dinámicas adicionales.
```cpp
#include <new>
#include <iostream>

struct Punto {
    int x, y;
};

int main() {
    char buffer[sizeof(Punto)]; // Buffer en el Stack
    
    // Placement New: Construye en el buffer local
    Punto* p = new (buffer) Punto{10, 20}; 
    
    std::cout << p->x << ", " << p->y << "\n";
    // IMPORTANTE: Con placement new, no se debe usar delete.
    // Se debe invocar el destructor manualmente si es necesario: p->~Punto();
    return 0;
}
```

### Nivel Complejo: Sobrecarga global de `operator new`
C++ nos permite interceptar todas las alocaciones dinámicas del programa sobrecargando el operador de forma global o local para depurar memory leaks o usar alocadores especializados.
```cpp
#include <iostream>
#include <new>

void* operator new(size_t size) {
    std::cout << "Interceptando alocacion de: " << size << " bytes.\n";
    if (void* ptr = std::malloc(size)) {
        return ptr;
    }
    throw std::bad_alloc();
}

void operator delete(void* ptr) noexcept {
    std::free(ptr);
}
```

---

## Diseño de Sistemas
En el desarrollo de software embebido crítico, no se permite el uso de `new` / `delete` por defecto. En su lugar, se implementan agrupaciones de memoria fija (Memory Pools) prealocadas en el arranque y se gestiona la construcción sobre ellas usando *Placement New*.

---

## Ejercicios

### Ejercicio 1 — Corregir Fuga de Memoria de Arreglo
Detecta el error en el uso de operadores de liberación en el siguiente fragmento y corrígelo para asegurar que se llamen todos los destructores.

```cpp
#include <iostream>

struct Item {
    Item() { std::cout << "Item creado.\n"; }
    ~Item() { std::cout << "Item destruido.\n"; }
};

int main() {
    // Alocación de un arreglo de Items
    Item* inventario = new Item[5];
    
    // TODO: Corregir esta llamada para que llame al destructor de los 5 elementos
    // delete inventario; 
    return 0;
}
```

---

## Errores Comunes y Anti-Patrones
- **Mezclar `new` con `free` o `malloc` con `delete`:** Rompe por completo el ciclo de vida del asignador de memoria y produce crashes inmediatos o corrupción silenciosa del Heap.
- **Usar el puntero tras destruirlo (Dangling Pointer):** Hacer `delete ptr;` no cambia el valor de `ptr` (sigue apuntando a la misma dirección física). Es una buena práctica poner a `nullptr` el puntero inmediatamente tras liberarlo: `ptr = nullptr;`.

---

## Conclusión y Checklist Mental
- [ ] ¿Entiendes por qué no debes usar `malloc` en C++ para instanciar objetos complejos?
- [ ] ¿Qué pasa si alocas con `new[]` pero liberas con `delete` común?
- [ ] ¿Qué es y cuándo se utiliza el operador *Placement New*?

---

*Siguiente tema sugerido: [21 — Sobrecarga de Operadores (operator)](21 — Polimorfismo y Funciones Virtuales.md)*





