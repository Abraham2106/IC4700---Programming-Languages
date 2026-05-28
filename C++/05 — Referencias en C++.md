# 05 — Referencias en C++

> **Resumen Ejecutivo:** Las referencias en C++ actúan como alias seguros de variables existentes. A diferencia de los punteros, no pueden ser reasignadas ni ser nulas. Introducen además la semántica de movimiento mediante las referencias rvalue (`&&`), eje de la eficiencia moderna.
>
> **Prerrequisitos:** Haber leído [04 — Punteros en C y C++](<04 — Punteros en C y C++.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Referencias Lvalue y Rvalue](#teoría-referencias-lvalue-y-rvalue)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
Una referencia es un nombre alternativo (un alias) para una variable que ya existe. En C++ existen dos categorías:
1. **Lvalue references (`T&`):** Apuntan a objetos persistentes en memoria (direcciones localizables).
2. **Rvalue references (`T&&`):** Apuntan a objetos temporales o literales destinados a ser destruidos.

### ¿Por qué importa?
- **Paso por referencia:** Evita la copia costosa de memoria sin la sintaxis compleja e insegura de los punteros.
- **Semántica de Movimiento:** Permite transferir recursos (memoria heap, handles) de un objeto a otro sin duplicar datos, mejorando drásticamente el rendimiento de C++ moderno.

---

## Conceptos Previos
- Comprender qué son el Lvalue (lado izquierdo de una asignación, con dirección física) y Rvalue (lado derecho, valor temporal).

---

## Hook Example

```cpp
#include <iostream>

void incrementar(int& ref) {
    ref++; // Modifica directamente la variable original sin usar desreferenciación explícita
}

int main() {
    int x = 10;
    int& ref = x; // ref es un alias de x
    
    incrementar(x);
    std::cout << "x vale: " << x << "\n"; // Imprime 11
    std::cout << "ref vale: " << ref << "\n"; // Imprime 11
    return 0;
}
```

---

## Descomposición Under the Hood

### ¿Qué hace el compilador con una referencia?
- A nivel del compilador, una referencia suele compilarse exactamente igual que un puntero constante (`T* const`).
- Sin embargo, a nivel de sintaxis del lenguaje, la desreferenciación ocurre de forma implícita. No se requiere el operador `*` para leer o modificar el valor apuntado, ni el operador `&` al pasar argumentos.
- **Optimizaciones:** En muchos casos, si la referencia es local, el compilador simplemente la elimina mediante el análisis de alias, vinculando directamente los accesos al nombre de la variable original.

---

## Teoría: Referencias Lvalue y Rvalue

### 1. Referencias Lvalue (`&`)
- Deben inicializarse inmediatamente al crearse.
- No pueden ser nulas.
- No pueden reasignarse para referenciar a otra variable distinta (toda asignación posterior altera el valor de la variable originalmente referenciada).

### 2. Referencias Rvalue (`&&`) - C++11
- Capturan temporales. Se usan principalmente para constructores de movimiento y operadores de asignación de movimiento.
- Permiten "robar" los recursos internos de un objeto temporal (como un buffer dinámico) porque sabemos que este temporal dejará de existir en breve.

---

## Progresión de Complejidad

### Nivel Simple: Paso de parámetros const por referencia
```cpp
// Rápido e inmutable. El estándar de C++ para tipos no primitivos.
void procesar(const std::string& texto); 
```

### Nivel Aplicado: Retornar Referencias
Se usa comúnmente en la sobrecarga de operadores para encadenar llamadas (ej. `operator<<`).
```cpp
class Contador {
    int valor = 0;
public:
    Contador& incrementar() {
        valor++;
        return *this; // Retorna referencia a sí mismo
    }
};
```

### Nivel Complejo: `std::move` y Rvalue References
`std::move` no mueve nada físicamente en runtime; es simplemente un cast en tiempo de compilación que convierte una variable Lvalue a una Rvalue reference (`T&&`), habilitando que sea consumida por un constructor de movimiento.
```cpp
#include <vector>
#include <string>

int main() {
    std::string original = "Hola Mundo";
    // Transfiere la propiedad del buffer interno del string
    std::string destino = std::move(original); 
    
    // original queda en un estado válido pero no especificado (usualmente vacío)
    return 0;
}
```

---

## Diseño de Sistemas
La semántica de movimiento implementada via referencias rvalue es lo que permite que clases como `std::unique_ptr` garanticen la propiedad exclusiva de un recurso en tiempo de compilación sin overhead de copia.

---

## Ejercicios

### Ejercicio 1 — Referencias vs Punteros
Corrige las siguientes declaraciones y llamadas de función para que utilicen referencias consistentes en lugar de punteros.

```cpp
#include <iostream>

// TODO: Reescribir esta función utilizando referencias en vez de punteros
void intercambiar(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main() {
    int x = 5, y = 10;
    
    // TODO: Ajustar la llamada para pasar las variables directamente por referencia
    intercambiar(&x, &y);
    
    std::cout << "x: " << x << ", y: " << y << "\n";
    return 0;
}
```

---

## Errores Comunes y Anti-Patrones
- **Retornar referencias a variables locales:**
  ```cpp
  int& obtener_inseguro() {
      int x = 10;
      return x; // ERROR: x se destruye al salir del scope. Produce dangling reference.
  }
  ```
- **Intentar reasignar referencias:** Escribir `ref = y;` no hace que `ref` apunte a `y`; en su lugar, asigna el valor de `y` a la variable a la que apuntaba `ref`.

---

## Conclusión y Checklist Mental
- [ ] ¿Entiendes por qué no existe una "referencia nula" legal en C++?
- [ ] ¿Sabes qué diferencia hay entre `std::move` y una copia física de datos?
- [ ] ¿Reconoces la diferencia entre un Lvalue y un Rvalue?

---

*Siguiente tema sugerido: [06 — Wrappers y Gestión de Recursos (RAII)](06 — Ámbito y Ciclo de Vida (Scope).md)*





