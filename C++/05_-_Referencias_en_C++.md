# Referencias en C++ — Semántica de alias, paso por referencia y optimización de copias

Las referencias en C++ introducen un mecanismo de alias seguro para variables existentes, eliminando la sobrecarga sintáctica y los riesgos de la aritmética de punteros. Al usarse en parámetros de funciones, permiten la manipulación de datos externos y evitan la copia costosa de objetos en el stack.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
Una referencia es un nombre alternativo (un alias) para una variable que ya existe. En C++ existen dos categorías:
1. **Lvalue references (`T&`):** Apuntan a objetos persistentes en memoria (direcciones localizables).
2. **Rvalue references (`T&&`):** Apuntan a objetos temporales o literales destinados a ser destruidos.

### 1.2 ¿Por qué importa?
- **Paso por referencia:** Evita la copia costosa de memoria sin la sintaxis compleja e insegura de los punteros.
- **Semántica de Movimiento:** Permite transferir recursos (memoria heap, handles) de un objeto a otro sin duplicar datos, mejorando drásticamente el rendimiento de C++ moderno.

---

## 2. Hook Example

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

## 3. Descomposición Under the Hood

### 3.1 ¿Qué hace el compilador con una referencia?
- A nivel del compilador, una referencia suele compilarse exactamente igual que un puntero constante (`T* const`).
- Sin embargo, a nivel de sintaxis del lenguaje, la desreferenciación ocurre de forma implícita. No se requiere el operador `*` para leer o modificar el valor apuntado, ni el operador `&` al pasar argumentos.
- **Optimizaciones:** En muchos casos, si la referencia es local, el compilador Únicamente la elimina mediante el análisis de alias, vinculando directamente los accesos al nombre de la variable original.

---

## 4. Teoría: Referencias Lvalue y Rvalue

### 4.1 1. Referencias Lvalue (`&`)
- Deben inicializarse inmediatamente al crearse.
- No pueden ser nulas.
- No pueden reasignarse para referenciar a otra variable distinta (toda asignación posterior altera el valor de la variable originalmente referenciada).

### 4.2 2. Referencias Rvalue (`&&`) - C++11
- Capturan temporales. Se usan principalmente para constructores de movimiento y operadores de asignación de movimiento.
- Permiten "robar" los recursos internos de un objeto temporal (como un buffer dinámico) porque sabemos que este temporal dejará de existir en breve.

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Paso de parámetros const por referencia
```cpp
// Rápido e inmutable. El estándar de C++ para tipos no primitivos.
void procesar(const std::string& texto); 
```

### 5.2 Nivel Aplicado: Retornar Referencias
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

### 5.3 Nivel Complejo: `std::move` y Rvalue References
`std::move` no mueve nada físicamente en runtime; es Únicamente un cast en tiempo de compilación que convierte una variable Lvalue a una Rvalue reference (`T&&`), habilitando que sea consumida por un constructor de movimiento.
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

## 6. Diseño de Sistemas
La semántica de movimiento implementada via referencias rvalue es lo que permite que clases como `std::unique_ptr` garanticen la propiedad exclusiva de un recurso en tiempo de compilación sin overhead de copia.

---

## Exercises

### Exercise 1 — Referencias vs Punteros
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

## 7. Errores Comunes y Anti-Patrones
- **Retornar referencias a variables locales:**
  ```cpp
  int& obtener_inseguro() {
      int x = 10;
      return x; // ERROR: x se destruye al salir del scope. Produce dangling reference.
  }
  ```
- **Intentar reasignar referencias:** Escribir `ref = y;` no hace que `ref` apunte a `y`; en su lugar, asigna el valor de `y` a la variable a la que apuntaba `ref`.

---

## 8. Conclusión

---

---

*Next: `06 — Wrappers y Gestión de Recursos (RAII).md` — Gestión automática y determinista del ciclo de vida de los recursos.*
