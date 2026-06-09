# Punteros en C y C++ — Aritmética de punteros, indirección y gestión de direcciones de memoria física

Los punteros son la abstracción directa de las celdas de memoria física direccionable en hardware. Su manipulación permite el acceso y control de datos a bajo nivel, pero introduce riesgos como desreferenciación nula o punteros colgantes. Su uso requiere precisión matemática sobre la dirección de los bloques asignados.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
Un puntero es una variable cuyo valor es la dirección física en la memoria RAM de otra variable u objeto.

### 1.2 ¿Por qué importa?
1. **Acceso directo al hardware:** Permite manipular posiciones de memoria directamente.
2. **Eficiencia:** Evita la copia de estructuras y objetos grandes al pasarlos a funciones.
3. **Estructuras de datos dinámicas:** Esencial para listas enlazadas, árboles, grafos y memoria en el Heap.

---

## 2. Hook Example

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

## 3. Descomposición Under the Hood

### 3.1 ¿Qué ocurre en la arquitectura de hardware?
- Si declaras `int x = 42;`, el compilador asigna una dirección (digamos, `0x7ffee3bf8ac8`) en el Stack y guarda los bits del entero `42` en esa celda de 4 bytes.
- Si creas `int* p = &x;`, el compilador reserva otra variable en el Stack (generalmente de 8 bytes en arquitecturas de 64 bits) y en ella almacena el número de dirección `0x7ffee3bf8ac8`.
- El operador de desreferenciación `*p` se traduce en código máquina a instrucciones de carga indirecta (`MOV` en x86-64 o `LDR` en ARM), indicando a la CPU que cargue el valor alojado en la dirección guardada dentro del registro del puntero.

---

## 4. Teoría: Direccionamiento y Punteros

### 4.1 1. Sintaxis Básica
- Operador de dirección (`&`): Obtiene la dirección de memoria de una variable.
- Operador de desreferencia (`*`): Accede al valor almacenado en la dirección apuntada.

### 4.2 2. Aritmética de Punteros
Sumar o restar a un puntero no incrementa los bytes en 1, sino en múltiplos del tamaño del tipo al que apunta:
```cpp
int arr[3] = {10, 20, 30};
int* p = arr; // apunta a arr[0]
p++; // Ahora apunta a arr[1] (se le sumaron sizeof(int) bytes, típicamente 4)
```

### 4.3 3. `nullptr` vs `NULL`
- En C clásico se usaba `NULL` (o `0`), lo que podía causar ambigüedades en sobrecargas de funciones que tomaban enteros.
- En C++ moderno (desde C++11) se usa `nullptr`, que tiene un tipo propio `std::nullptr_t` y no se convierte implícitamente a enteros.

### 4.4 4. Puntero genérico `void*`
- Es un puntero que apunta a datos de tipo desconocido. No se puede desreferenciar directamente sin antes hacer un cast explícito (`static_cast`).

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Paso por puntero a funciones
```cpp
void duplicar(int* n) {
    if (n) { // Siempre verificar null pointers
        *n *= 2;
    }
}
```

### 5.2 Nivel Aplicado: Puntero a Puntero (Double Pointer)
Se usa comúnmente para modificar la dirección a la que apunta un puntero desde dentro de una función auxiliar.
```cpp
void alocar_entero(int** double_ptr) {
    *double_ptr = new int(100); // Modifica el puntero original
}
```

### 5.3 Nivel Complejo: Puntero a Funciones
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

## 6. Diseño de Sistemas
En el desarrollo de kernels o APIs de sistemas operativos, los punteros a funciones son el núcleo para implementar callbacks y tablas de despacho de controladores de dispositivos.

---

## Exercises

### Exercise 1 — Aritmética de Punteros Manual
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

## 7. Errores Comunes y Anti-Patrones
- **Dangling Pointer (Puntero colgado):** Apuntar a una dirección de memoria liberada o fuera de ámbito.
- **Buffer Overflow:** Escribir más allá del límite de un arreglo usando aritmética de punteros insegura.
- **Memory Leak:** Reasignar un puntero que apuntaba a memoria del Heap sin llamar antes a `delete`.

---

## 8. Conclusión

---

---

*Next: `05 — Referencias en C++.md` — Alias y paso por referencia sin copias innecesarias.*
