# 11 — Sobrecarga (Overloading)

> **Resumen Ejecutivo:** La sobrecarga en C++ permite que múltiples funciones o constructores compartan el mismo nombre siempre que sus firmas de argumentos difieran. El compilador resuelve estáticamente qué función llamar en tiempo de compilación mediante el mecanismo de Name Mangling (decoración de nombres).
>
> **Prerrequisitos:** Haber leído [08 — Funciones y Operadores de Acceso](<08 — Funciones y Operadores de Acceso.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Firmas y Name Mangling](#teoría-firmas-y-name-mangling)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
La sobrecarga de funciones es la capacidad de definir varias funciones con el mismo nombre dentro del mismo ámbito, diferenciándolas únicamente por el número, tipo u orden de sus parámetros.

### ¿Por qué importa?
- **Sintaxis unificada:** Permite usar un solo nombre lógico (ej. `imprimir`) para conceptualizar la misma acción sobre diferentes tipos de datos, sin necesidad de crear funciones con nombres redundantes (como `imprimir_int`, `imprimir_float`, `imprimir_string` en C).
- **Polimorfismo estático:** Habilita el despacho estático (compile-time dispatch), lo que significa cero costo de rendimiento en tiempo de ejecución.

---

## Conceptos Previos
- Firma de función (nombre, parámetros y calificadores de const).
- Qué es la sobrecarga en comparación con la sobrescritura (override).

---

## Hook Example

```cpp
#include <iostream>
#include <string>

// Sobrecarga 1: Recibe un entero
void imprimir(int x) {
    std::cout << "Entero: " << x << "\n";
}

// Sobrecarga 2: Recibe un string
void imprimir(const std::string& s) {
    std::cout << "String: " << s << "\n";
}

int main() {
    imprimir(42); // Resuelve a la Sobrecarga 1
    imprimir("Hola"); // Resuelve a la Sobrecarga 2 (mediante conversión implícita a std::string)
    return 0;
}
```

---

## Descomposición Under the Hood

### Name Mangling (Decoración de nombres)
- En C, el enlazador no tolera nombres de funciones duplicados en la tabla de símbolos del ejecutable.
- Para solucionar esto en C++, el compilador altera internamente el nombre final de la función codificando la información de los parámetros en el símbolo resultante. Esto es el **Name Mangling**.

Por ejemplo:
- `void procesar(int)` se convierte internamente en algo similar a: `__Z8procesari`
- `void procesar(double, int)` se convierte en: `__Z8procesardi`
- Al enlazarse el binario, el Linker no ve funciones duplicadas, sino símbolos únicos.
- Si queremos enlazar código C++ con C clásico, debemos desactivar este comportamiento declarando la función dentro de un bloque `extern "C"`.

---

## Teoría: Firmas y Name Mangling

### 1. ¿Qué constituye una sobrecarga válida?
- El compilador diferencia funciones por:
  - Tipo de argumentos (`int` vs `double`).
  - Número de argumentos (`int` vs `int, int`).
  - Calificadores de const y volatilidad (`const T&` vs `T&`).

- **IMPORTANTE:** El tipo de retorno de la función **no** se considera parte de la firma para la resolución de sobrecargas. Declarar dos funciones que solo difieren en el tipo de retorno causa un error de compilación inmediato.

### 2. Sobrecarga por const correctness
Podemos sobrecargar métodos de clase basados en si la instancia actual es `const` o no:
```cpp
class Contenedor {
    int datos[10];
public:
    // Para instancias mutables: permite lectura y escritura
    int& operator[](int idx) { return datos[idx]; }
    
    // Para instancias constantes: solo lectura
    const int& operator[](int idx) const { return datos[idx]; }
};
```

---

## Progresión de Complejidad

### Nivel Simple: Sobrecarga básica
```cpp
int sumar(int a, int b) { return a + b; }
double sumar(double a, double b) { return a + b; }
```

### Nivel Aplicado: Funciones con argumentos por defecto
Las funciones con argumentos predeterminados pueden generar conflictos de ambigüedad si no se diseñan con cuidado.
```cpp
void registrar(std::string nombre, int id = 0);
void registrar(std::string nombre); // ERROR: registrar("Kael") causa ambigüedad en compilación.
```

### Nivel Complejo: Sobrecarga por calificación Ref (Ref-Qualifiers) - C++11
C++ permite sobrecargar métodos de una clase dependiendo de si el objeto emisor es un Lvalue o un Rvalue.
```cpp
#include <iostream>
#include <vector>

class Procesador {
    std::vector<int> datos;
public:
    // Se ejecuta si el objeto emisor es un Lvalue (objeto persistente)
    std::vector<int> obtener_datos() & {
        std::cout << "Copiando datos...\n";
        return datos;
    }
    
    // Se ejecuta si el objeto emisor es un Rvalue (objeto temporal)
    std::vector<int> obtener_datos() && {
        std::cout << "Moviendo datos...\n";
        return std::move(datos); // Evita copias físicas
    }
};
```

---

## Diseño de Sistemas
En el diseño de frameworks, la sobrecarga es la base sobre la que se construyen las APIs de serialización (ej. serializar enteros, flotantes, y estructuras complejas a un buffer de salida con la misma llamada simple `escribir(dato)`).

---

## Ejercicios

### Ejercicio 1 — Sobrecarga y Ambigüedad de Tipos
Añade las sobrecargas necesarias para evitar conversiones implícitas no deseadas. Explica por qué ocurre un error de ambigüedad al llamar a la función con un tipo de dato `float` si solo existen sobrecargas para `int` y `double`.

```cpp
#include <iostream>

void procesar_numero(int x) {
    std::cout << "Procesando entero: " << x << "\n";
}

void procesar_numero(double x) {
    std::cout << "Procesando double: " << x << "\n";
}

// TODO: Crear una sobrecarga para float para resolver llamadas directas con float

int main() {
    procesar_numero(10);     // Resuelve a int
    procesar_numero(10.5);   // Resuelve a double
    
    // procesar_numero(10.5f); // ¿Qué pasa aquí? Descomenta e intenta compilar sin la sobrecarga de float
    return 0;
}
```

---

## Errores Comunes y Anti-Patrones
- **Intentar sobrecargar basándose en el tipo de retorno:**
  ```cpp
  int obtener();
  double obtener(); // ERROR: El compilador no puede decidir cuál llamar en base al contexto de uso.
  ```
- **Confundir sobrecarga con promoción de tipos implícitos:** Pasar un tipo `char` a una función que solo tiene sobrecargas para `int` y `double` compilará debido a la promoción automática a `int`, lo que puede no ser el comportamiento deseado.

---

## Conclusión y Checklist Mental
- [ ] ¿Entiendes por qué el tipo de retorno no sirve para diferenciar una sobrecarga?
- [ ] ¿Qué es el Name Mangling y cómo ayuda al enlazador?
- [ ] ¿Cómo puedes deshabilitar el Name Mangling en C++ (`extern "C"`)?

---

*Siguiente tema sugerido: [12 — Herencia y Visibilidad](<12 — Herencia y Visibilidad.md>)*


