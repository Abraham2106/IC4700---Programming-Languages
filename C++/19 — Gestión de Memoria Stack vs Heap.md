# 19 — Gestión de Memoria Stack vs Heap

> **Resumen Ejecutivo:** La memoria RAM disponible para los programas en C++ se divide conceptualmente en dos regiones principales: el Stack (Pila) y el Heap (Montículo). Comprender sus diferencias operativas es crucial para balancear la velocidad, el tamaño de los datos y el control del ciclo de vida físico del software.
>
> **Prerrequisitos:** Haber leído [07 — Ámbito y Ciclo de Vida (Scope)](07 — Funciones y Operadores de Acceso.md).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Stack vs Heap](#teoría-stack-vs-heap)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
- **Stack (Pila):** Memoria de acceso estructurado LIFO (Last In, First Out) gestionada automáticamente por la CPU.
- **Heap (Montículo):** Región de memoria libre de propósito general gestionada dinámicamente por el programador (vía alocaciones explícitas).

### ¿Por qué importa?
- **Rendimiento:** Las alocaciones en el Stack tardan fracciones de nanosegundo. Las alocaciones en el Heap requieren llamadas al sistema (sys-calls) y algoritmos de búsqueda que pueden ser miles de veces más lentos.
- **Limitación física:** El Stack tiene un tamaño limitado por el sistema operativo (típicamente 1MB a 8MB). Exceder este límite causa crashes catastróficos inmediatos por desbordamiento de pila (Stack Overflow). El Heap solo está limitado por la RAM total física y virtual del equipo.

---

## Conceptos Previos
- Qué es un puntero conceptualmente.
- Concepto de llamadas al sistema operativo (syscalls).

---

## Hook Example

```cpp
#include <iostream>

void funcion_stack() {
    int x = 10; // Alocado en el Stack. Liberado automáticamente al retornar.
}

void funcion_heap() {
    int* p = new int(10); // Alocado en el Heap.
    // ... usar p ...
    delete p; // Debe ser liberado manualmente; de lo contrario, fuga de memoria (leak).
}

int main() {
    funcion_stack();
    funcion_heap();
    return 0;
}
```

---

## Descomposición Under the Hood

### ¿Cómo opera la CPU en cada memoria?

#### 1. En el Stack:
- La CPU utiliza un registro interno de propósito especial llamado **Stack Pointer** (por ejemplo, `RSP` en x86-64).
- Alocar memoria consiste simplemente en restar un offset al `RSP` (ej. `SUB RSP, 16` para reservar 16 bytes).
- Liberar consiste en volver a sumarle el offset (`ADD RSP, 16`). Es una sola instrucción básica de CPU a velocidad del hardware.
- Los accesos a memoria Stack son altamente localizados, lo que aprovecha al máximo la caché L1/L2 de la CPU.

#### 2. En el Heap:
- Al invocar `new` o `malloc`, la biblioteca de runtime de C++ debe interactuar con el asignador de memoria (Memory Allocator).
- El asignador busca en una lista de bloques libres un espacio de tamaño adecuado (algoritmos como First-Fit o Best-Fit).
- Si no encuentra espacio libre en su caché de páginas, debe realizar una llamada al sistema (`brk` o `mmap` en Linux, `VirtualAlloc` en Windows) para solicitar páginas de memoria física adicionales al sistema operativo. Esto requiere pausar el hilo, cambiar de modo de ejecución (de usuario a kernel) y alterar las tablas de páginas de la MMU.

---

## Teoría: Stack vs Heap

### Tabla Comparativa:

| Aspecto | Stack (Pila) | Heap (Montículo) |
|---|---|---|
| **Gestión** | Automática por la CPU | Manual por el programador |
| **Velocidad** | Extremadamente rápida (un ciclo de CPU) | Moderada a lenta (búsqueda de bloques, syscalls) |
| **Tamaño disponible** | Limitado (típicamente de 1MB a 8MB) | Flexible (limitado por la memoria física libre) |
| **Acceso a datos** | Directo | Indirecto (a través de punteros) |
| **Fragmentación** | Imposible (acceso contiguo LIFO) | Posible (bloques libres dispersos en el tiempo) |
| **Orden de Vida** | Estricto (LIFO por llaves de bloque `{}`) | Arbitrario (persiste hasta que se llame a `delete`) |

---

## Progresión de Complejidad

### Nivel Simple: Alocar arreglos
```cpp
// Stack: Rápido pero peligroso si el tamaño es muy grande
int arr_stack[1000]; 

// Heap: Seguro contra desbordamientos de pila
int* arr_heap = new int[1000000]; 
delete[] arr_heap;
```

### Nivel Aplicado: Reducir copias usando Semántica de Valor en Stack
C++ moderno prefiere pasar objetos por valor si el compilador puede optimizarlos mediante RVO (Return Value Optimization) y semántica de movimiento, eliminando la necesidad de alocar objetos en el Heap solo para "evitar copias".
```cpp
#include <vector>

std::vector<int> obtener_datos() {
    std::vector<int> temp = {1, 2, 3};
    return temp; // El compilador optimiza esto y lo coloca directamente en el destino (RVO).
}
```

---

## Diseño de Sistemas
En el diseño de sistemas en tiempo real (ej. sistemas automotrices o médicos), el uso del Heap está estrictamente prohibido o limitado a la fase de arranque de la aplicación. Esto se debe a que el tiempo de alocación en el Heap es no determinista (no se puede garantizar que tardará menos de $X$ microsegundos en resolver).

---

## Ejercicios

### Ejercicio 1 — Provocar un Stack Overflow Controlado
Escribe una función recursiva infinita para desbordar intencionalmente la memoria Stack. Ejecuta y comprueba cómo el sistema operativo detiene la ejecución inmediatamente.

```cpp
#include <iostream>

void llamada_recursiva(int profundidad) {
    // TODO: Alocar un array grande en el Stack en cada llamada para acelerar el desbordamiento
    char buffer[1024]; // 1 KB
    
    // Imprimir el nivel actual de profundidad para ver hasta dónde llegó
    std::cout << "Profundidad: " << profundidad << "\n";
    
    // Llamada recursiva infinita
    llamada_recursiva(profundidad + 1);
}

int main() {
    // llamada_recursiva(1);
    return 0;
}
```

---

## Errores Comunes y Anti-Patrones
- **Memory Leak (Fuga de memoria):** Perder la dirección de un bloque del Heap sin haber invocado a `delete`. El bloque permanece ocupado e inaccesible hasta que el programa termina.
- **Double Free:** Liberar dos veces el mismo puntero en el Heap. Corrompe los metadatos internos del asignador de memoria y causa vulnerabilidades graves de seguridad.

---

## Conclusión y Checklist Mental
- [ ] ¿Entiendes por qué alocar en el Stack es miles de veces más rápido que en el Heap?
- [ ] ¿Qué es y qué provoca un Stack Overflow?
- [ ] ¿Por qué los sistemas en tiempo real evitan el uso del Heap durante la fase de ejecución en caliente?

---

*Siguiente tema sugerido: [20 — Operadores new y delete](<20 — Sobrecarga de Operadores (operator).md>)*





