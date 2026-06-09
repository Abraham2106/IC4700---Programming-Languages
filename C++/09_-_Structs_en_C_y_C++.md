# Structs en C y C++ — De contenedores de datos pasivos a clases con comportamiento y herencia

Mientras que en C un struct es una simple agrupación pasiva de variables contiguas en memoria, en C++ se eleva a una clase con visibilidad de miembros por defecto pública. Esta transición unifica el diseño de estructuras de datos con el soporte para comportamiento y encapsulación.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
Un `struct` es un tipo de dato definido por el usuario que agrupa variables lógicamente relacionadas (miembros) bajo un solo nombre de tipo.

### 1.2 ¿Por qué importa?
- **Modelado de datos:** Permite estructurar registros e información compleja.
- **Diferencia conceptual fundamental:** Confundir el comportamiento de un `struct` en C con el de C++ puede impedir la creación de código limpio y eficiente, o causar malentendidos al integrar APIs de C legado en C++ moderno.

---

## 2. Hook Example

```c
// C clásico: Solo datos. Requiere usar typedef o repetir la palabra struct.
struct PuntoC {
    int x;
    int y;
};
// struct PuntoC p = {10, 20}; // Sintaxis C crudo
```

```cpp
// C++ moderno: El struct puede tener constructores, destructores y métodos.
struct PuntoCpp {
    int x;
    int y;

    PuntoCpp(int px, int py) : x(px), y(py) {} // Constructor legal en C++

    void imprimir() const {
        std::cout << x << ", " << y << "\n"; // Método de clase legal en C++
    }
};
// PuntoCpp p(10, 20); // Inicialización directa
```

---

## 3. Descomposición Under the Hood

### 3.1 Memoria, Alineación y Relleno (Padding)
- Los miembros de un `struct` se colocan secuencialmente en la memoria en el orden en que se declaran.
- Sin embargo, la CPU lee la memoria más eficientemente en bloques alineados con su tamaño de palabra (generalmente de 4 u 8 bytes).
- Para garantizar esta alineación, el compilador inyecta silenciosamente bytes vacíos de relleno (padding) entre los miembros de la estructura.

```cpp
struct Mixto {
    char a;      // 1 byte
    // 3 bytes de padding inyectados aquí
    int b;       // 4 bytes (debe alinearse a una dirección múltiplo de 4)
    char c;      // 1 byte
    // 3 bytes de padding al final para alinear el tamaño total de la estructura a 12 bytes
};
// sizeof(Mixto) dará 12 en vez de los 6 bytes teóricos.
```
- Reordenar miembros de mayor a menor tamaño optimiza el padding consumido en memoria.

---

## 4. Teoría: Structs en C vs C++

### 4.1 1. En C:
- No puede contener funciones miembro ni métodos.
- No soporta herencia ni polimorfismo.
- No soporta control de acceso (`public`, `private`). Todos los datos son expuestos públicamente.
- Para usar el tipo sin escribir la palabra clave `struct` cada vez, se requiere declarar un alias con `typedef`.

### 4.2 2. En C++:
- Un `struct` es exactamente idéntico a una clase (`class`) con **una sola diferencia:**
  - En un `struct`, el acceso por defecto para sus miembros e interfaces de herencia es **`public`**.
  - En un `class`, el acceso por defecto es **`private`**.
- Por convención en C++ moderno, se prefiere utilizar `struct` para estructuras puras de datos pasivos (Plain Old Data - POD) y `class` para objetos con comportamiento complejo y encapsulamiento estricto.

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Inicialización Agregada (Aggregate Initialization)
C++ permite inicializar structs de manera compacta usando llaves sin requerir declarar constructores explícitos.
```cpp
struct Color {
    int r;
    int g;
    int b;
};

Color rojo{255, 0, 0}; // Inicialización por orden de declaración
```

### 5.2 Nivel Aplicado: Struct Packeado
Cuando necesitamos transmitir estructuras por red o escribir en archivos binarios con tamaños exactos sin bytes de padding extras, podemos desactivar la alineación del compilador.
```cpp
#pragma pack(push, 1) // Fuerza alineación a 1 byte
struct PaqueteRed {
    char id;
    int valor;
}; // sizeof(PaqueteRed) es exactamente 5 bytes
#pragma pack(pop) // Restaura configuración original
```

### 5.3 Nivel Complejo: Struct como Functor
En C++, dado que un `struct` es una clase, puede sobrecargar el operador de llamada a función `operator()` para actuar como una función con estado (functor).
```cpp
#include <iostream>

struct Multiplicador {
    int factor;
    int operator()(int valor) const {
        return valor * factor;
    }
};

int main() {
    Multiplicador triplicar{3};
    std::cout << triplicar(10) << "\n"; // Llama a operator() e imprime 30
    return 0;
}
```

---

## 6. Diseño de Sistemas
En el diseño de motores de videojuegos o bases de datos relacionales, reordenar los miembros de un `struct` para optimizar su tamaño físico puede reducir dramáticamente la tasa de fallos de caché de la CPU (cache misses) al procesar millones de instancias en caliente.

---

## Exercises

### Exercise 1 — Optimizar Estructura de Memoria
Reordena los campos de la siguiente estructura para minimizar el tamaño físico en bytes debido al padding del compilador. Documenta el tamaño antes y después.

```cpp
#include <iostream>

// TODO: Reordenar los miembros para optimizar el alineamiento de memoria
struct SensorData {
    char tipo_sensor;   // 1 byte
    double lectura;     // 8 bytes
    bool estado;        // 1 byte
    int id;             // 4 bytes
};

int main() {
    std::cout << "Tamano SensorData: " << sizeof(SensorData) << " bytes\n";
    // El tamaño debería reducirse si agrupas adecuadamente por tamaño de datos.
    return 0;
}
```

---

## 7. Errores Comunes y Anti-Patrones
- **Usar funciones de copia estilo C (`memcpy`) en structs de C++:** Hacer `memcpy` sobre un `struct` que contiene tipos dinámicos de C++ (como `std::string` o `std::vector`) corrompe la tabla interna de punteros del objeto, provocando memory leaks o crashes inmediatos al invocarse el destructor.
- **Olvidar el punto y coma `;` al cerrar el bloque de un struct:** Es un error clásico de compilación en C/C++ que suele desatar decenas de errores sintácticos confusos en cascada en las líneas posteriores del archivo.

---

## 8. Conclusión

---

---

*Next: `10 — Global Scope y Objetos Globales.md` — Variables globales y su orden de inicialización.*
