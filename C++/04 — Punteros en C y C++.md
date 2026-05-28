# 04 — Punteros en C y C++

> **Resumen Ejecutivo:** Los punteros son variables que almacenan direcciones de memoria. Entender los punteros es la clave para dominar el bajo nivel, la aritmética de direcciones, la manipulación de arreglos y la alocación dinámica de memoria.
>
> **Prerrequisitos:** Haber leído [01 — Diferencias entre C, C++ y Rust](<C vs C++ vs Rust.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Direccionamiento y Punteros](#teoría-direccionamiento-y-punteros)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
Un puntero es una variable cuyo valor es la dirección física en la memoria RAM de otra variable u objeto.

### ¿Por qué importa?
1. **Acceso directo al hardware:** Permite manipular posiciones de memoria directamente.
2. **Eficiencia:** Evita la copia de estructuras y objetos grandes al pasarlos a funciones.
3. **Estructuras de datos dinámicas:** Esencial para listas enlazadas, árboles, grafos y memoria en el Heap.

---

## Conceptos Previos
- Concepto de memoria RAM estructurada en celdas indexadas.
- La diferencia entre el valor de una variable y su dirección en memoria.

---

## Hook Example

```cpp
#include <iostream>

int main() {
    int x = 42;
    int* p = &x; // p almacena la dirección de memoria de x

    std::cout << "Valor de x: " << x << "\n";
    std::cout << "Direccion de x (&x): " << &x << "\n";
    std::cout << "Valor de p: " << p << "\n";
    std::cout << "Valor apuntado por p (*p): " << *p << "\n"; // Desreferenciación

    *p = 99; // Modifica el valor de x indirectamente
    std::cout << "Nuevo valor de x: " << x << "\n";
    return 0;
}
```

---

## Descomposición Under the Hood

### ¿Qué ocurre en la arquitectura de hardware?
- Si declaras `int x = 42;`, el compilador asigna una dirección (digamos, `0x7ffee3bf8ac8`) en el Stack y guarda los bits del entero `42` en esa celda de 4 bytes.
- Si creas `int* p = &x;`, el compilador reserva otra variable en el Stack (generalmente de 8 bytes en arquitecturas de 64 bits) y en ella almacena el número de dirección `0x7ffee3bf8ac8`.
- El operador de desreferenciación `*p` se traduce en código máquina a instrucciones de carga indirecta (`MOV` en x86-64 o `LDR` en ARM), indicando a la CPU que cargue el valor alojado en la dirección guardada dentro del registro del puntero.

---

## Teoría: Direccionamiento y Punteros

### 1. Sintaxis Básica
- Operador de dirección (`&`): Obtiene la dirección de memoria de una variable.
- Operador de desreferencia (`*`): Accede al valor almacenado en la dirección apuntada.

### 2. Aritmética de Punteros
Sumar o restar a un puntero no incrementa los bytes en 1, sino en múltiplos del tamaño del tipo al que apunta:
```cpp
int arr[3] = {10, 20, 30};
int* p = arr; // apunta a arr[0]
p++; // Ahora apunta a arr[1] (se le sumaron sizeof(int) bytes, típicamente 4)
```

### 3. `nullptr` vs `NULL`
- En C clásico se usaba `NULL` (o `0`), lo que podía causar ambigüedades en sobrecargas de funciones que tomaban enteros.
- En C++ moderno (desde C++11) se usa `nullptr`, que tiene un tipo propio `std::nullptr_t` y no se convierte implícitamente a enteros.

### 4. Puntero genérico `void*`
- Es un puntero que apunta a datos de tipo desconocido. No se puede desreferenciar directamente sin antes hacer un cast explícito (`static_cast`).

---

## Progresión de Complejidad

### Nivel Simple: Paso por puntero a funciones
```cpp
void duplicar(int* n) {
    if (n) { // Siempre verificar null pointers
        *n *= 2;
    }
}
```

### Nivel Aplicado: Puntero a Puntero (Double Pointer)
Se usa comúnmente para modificar la dirección a la que apunta un puntero desde dentro de una función auxiliar.
```cpp
void alocar_entero(int** double_ptr) {
    *double_ptr = new int(100); // Modifica el puntero original
}
```

### Nivel Complejo: Puntero a Funciones
Permite inyectar comportamiento dinámicamente pasándolo como argumento.
```cpp
#include <iostream>

void saludar() { std::cout << "Hola!\n"; }

int main() {
    void (*ptr_funcion)() = &saludar; // Firma exacta de la funcion
    ptr_funcion(); // Ejecuta la función apuntada
}
```

---

## Diseño de Sistemas
En el desarrollo de kernels o APIs de sistemas operativos, los punteros a funciones son el núcleo para implementar callbacks y tablas de despacho de controladores de dispositivos.

---

## Ejercicios

### Ejercicio 1 — Aritmética de Punteros Manual
Dado el siguiente arreglo, completa el código usando solo aritmética de punteros para imprimir los elementos en orden inverso.

```cpp
#include <iostream>

int main() {
    int arr[] = {10, 20, 30, 40, 50};
    int len = sizeof(arr) / sizeof(arr[0]);
    
    // TODO: Apuntar al último elemento usando aritmética de punteros
    int* ptr = arr + (len - 1);
    
    for (int i = 0; i < len; i++) {
        // TODO: Imprimir el elemento actual y decrementar el puntero
        std::cout << *ptr << " ";
        ptr--;
    }
    std::cout << "\n";
    return 0;
}
```

---

## Errores Comunes y Anti-Patrones
- **Dangling Pointer (Puntero colgado):** Apuntar a una dirección de memoria liberada o fuera de ámbito.
- **Buffer Overflow:** Escribir más allá del límite de un arreglo usando aritmética de punteros insegura.
- **Memory Leak:** Reasignar un puntero que apuntaba a memoria del Heap sin llamar antes a `delete`.

---

## Conclusión y Checklist Mental
- [ ] ¿Puedes explicar qué es `nullptr` y por qué es superior a `NULL`?
- [ ] ¿Entiendes cómo se incrementa la dirección física al realizar `p++` sobre un puntero de tipo `double`?
- [ ] ¿Identificas cuándo un puntero se convierte en un dangling pointer?

---

*Siguiente tema sugerido: [05 — Referencias en C++](<05 — Referencias en C++.md>)*


