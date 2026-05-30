# Operadores new y delete — Gestión de memoria dinámica nativa

Los operadores `new` y `delete` gestionan el ciclo de vida del heap al alocar memoria física e invocar constructores y destructores de forma acoplada. Su uso inapropiado produce fugas de memoria o corrupción por doble liberación.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
- **`new`:** Operador que reserva memoria física en el Heap e inicializa el objeto llamando a su constructor.
- **`delete`:** Operador que invoca al destructor del objeto y devuelve la memoria física al asignador.

### 1.2 ¿Por qué importa?
- **Constructor Trigger:** Usar funciones de C como `malloc` en C++ reserva la memoria física pero deja los miembros internos sin inicializar (no llama al constructor). Esto provoca un comportamiento indefinido (UB) inmediato al intentar leer miembros complejos como `std::string`.
- **Memory leaks:** La memoria del Heap no se autoliberará jamás al salir de ámbito. Es imperativo que cada llamada a `new` tenga su correspondiente llamada a `delete` (o usar Smart Pointers).

---

## 2. Hook Example

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

## 3. Descomposición Under the Hood

### 3.1 ¿Qué traduce el compilador al ver `new` y `delete`?
- La llamada `Usuario* u = new Usuario("Arthas");` se expande internamente a:
  1. **Llamada de Alocación:** `void* memoria = operator new(sizeof(Usuario));` (reserva de memoria física).
  2. **Llamada de Construcción:** Ejecuta el constructor de `Usuario` sobre la dirección apuntada por `memoria` usando una instrucción especial llamada *Placement New*.
- La llamada `delete u;` se expande a:
  1. **Llamada de Destrucción:** Llama explícitamente al destructor `u->~Usuario();`.
  2. **Llamada de Liberación:** `operator delete(u);` (retorna los bytes al allocator).

---

## 4. Teoría: Alocación Dinámica C vs C++

### 4.1 Comparativa:
| Característica | `malloc` / `free` | `new` / `delete` |
|---|---|---|
| **Tipo de entidad** | Funciones de biblioteca (`<stdlib.h>`) | Palabras clave / Operadores nativos de C++ |
| **Invocación de Constructores** | No | Sí (Automática) |
| **Type Safety** | Retorna `void*` (Requiere cast explícito) | Retorna tipo tipado seguro directamente |
| **Cálculo de tamaño** | Manual via `sizeof(T)` | Automático por el compilador |
| **Manejo de fallos** | Retorna `nullptr` | Lanza excepción `std::bad_alloc` por defecto |

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Alocación de Arreglos (`new[]` y `delete[]`)
Si alocas un arreglo dinámico usando corchetes, debes usar obligatoriamente la versión de liberación con corchetes.
```cpp
int* arr = new int[50];
delete[] arr; // Correcto. delete arr; causa comportamiento indefinido.
```

### 5.2 Nivel Aplicado: Placement New
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

### 5.3 Nivel Complejo: Sobrecarga global de `operator new`
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

## 6. Diseño de Sistemas
En el desarrollo de software embebido crítico, no se permite el uso de `new` / `delete` por defecto. En su lugar, se implementan agrupaciones de memoria fija (Memory Pools) prealocadas en el arranque y se gestiona la construcción sobre ellas usando *Placement New*.

---

## Exercises

### Exercise 1 — Corregir Fuga de Memoria de Arreglo
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

## 7. Errores Comunes y Anti-Patrones
- **Mezclar `new` con `free` o `malloc` con `delete`:** Rompe por completo el ciclo de vida del asignador de memoria y produce crashes inmediatos o corrupción silenciosa del Heap.
- **Usar el puntero tras destruirlo (Dangling Pointer):** Hacer `delete ptr;` no cambia el valor de `ptr` (sigue apuntando a la misma dirección física). Es una buena práctica poner a `nullptr` el puntero inmediatamente tras liberarlo: `ptr = nullptr;`.

---

## 8. Conclusión

---

---

*Next: `21 — Sobrecarga de Operadores (operator).md` — Extender la sintaxis nativa a tipos de usuario con consistencia semántica.*
