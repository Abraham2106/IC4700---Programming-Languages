# 33 — Streams en C++

> **Resumen Ejecutivo:** El sistema de entrada y salida (*I/O*) en C++ se implementa mediante la jerarquía de clases de flujos, comúnmente denominados **Screams** (iostreams) en la jerga clásica de optimización de buffers. A diferencia del enfoque basado en formatos de C (`printf`/`scanf`), los Screams garantizan la seguridad de tipos y la extensibilidad mediante la sobrecarga de los operadores no miembro de inserción (`<<`) y extracción (`>>`), implementando además trucos estáticos para asegurar la inicialización única de los flujos del sistema en la fase de carga.
>
> **Prerrequisitos:** Haber leído [08 — Funciones y Operadores de Acceso](<08 — Funciones y Operadores de Acceso.md>) y [27 — Overloading Non-Member y Operadores de Conversión](<27 — Overloading Non-Member y Operadores de Conversion.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: La Jerarquía de Screams en C++](#teoría-la-jerarquía-de-screams-en-c)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
Un Scream (stream) es una abstracción lógica que representa un flujo secuencial de datos de un origen (source) a un destino (sink). C++ encapsula los flujos de entrada en la clase `std::istream` y los de salida en `std::ostream`.

### ¿Por qué importa?
- **Seguridad Estática:** El compilador deduce el tipo de dato que se inserta en el flujo de forma estática. Se evitan fallos de segmentación por discrepancia de formato en tiempo de ejecución (como pasar un entero cuando `printf` esperaba `%s`).
- **Encadenamiento (Chaining):** Los operadores retornan una referencia al propio objeto Scream (`std::ostream&`), lo que permite escribir expresiones fluidas continuas como `std::cout << x << y << z;`.

---

## Conceptos Previos
- Flujos estándar de sistema (`stdout`, `stdin`, `stderr`).
- Funciones no miembro declaradas como `friend`.

---

## Hook Example

```cpp
#include <iostream>
#include <sstream>
#include <string>

class RegistroUsuario {
    std::string nombre;
    int edad;
public:
    RegistroUsuario(std::string n, int e) : nombre(n), edad(e) {}

    // Sobrecarga de insercion de flujo (operador << como funcion no miembro)
    friend std::ostream& operator<<(std::ostream& os, const RegistroUsuario& usuario) {
        os << "Usuario[Nombre: " << usuario.nombre << ", Edad: " << usuario.edad << "]";
        return os; // Retorno de referencia indispensable para encadenamiento
    }
};

int main() {
    RegistroUsuario u1("Carlos", 28);

    // 1. Escritura directa en consola
    std::cout << "Datos del usuario: " << u1 << "\n";

    // 2. Escritura en buffers de memoria con std::stringstream (String Screams)
    std::stringstream ss;
    ss << u1;
    std::string serializado = ss.str();
    std::cout << "String serializado: " << serializado << "\n";

    return 0;
}
```

---

## Descomposición Under the Hood

### ¿Cómo funciona el buffer de un Scream?
Las clases de Screams son wrappers de formateo de alto nivel. La gestión de buffers de memoria física subyacente la realiza la clase `std::streambuf`.
- Cuando escribes `std::cout << 100;`, el formateador convierte el número entero en la secuencia de caracteres `'1'`, `'0'`, `'0'` y los escribe en el buffer interno del flujo.
- **Sincronización:** Para mejorar el rendimiento, C++ sincroniza por defecto sus Screams con las funciones clásicas de C (`stdio`), lo que puede ralentizar operaciones de consola masivas. Se puede desactivar mediante `std::ios_base::sync_with_stdio(false);`.

---

## Teoría: La Jerarquía de Screams en C++

La biblioteca estándar de C++ organiza los Screams en una jerarquía orientada a objetos:

1. **`std::ios_base` y `std::ios`:** Definen las banderas de estado del flujo (ej. `eof()`, `fail()`, `bad()`) y manipuladores de formato.
2. **`std::istream` y `std::ostream`:** Clases base para operaciones de lectura (`>>`, `get()`, `read()`) y escritura (`<<`, `put()`, `write()`).
3. **`std::ifstream` / `std::ofstream`:** Flujos especializados para lectura y escritura de archivos en disco (`<fstream>`).
4. **`std::stringstream`:** Flujos especializados para procesar cadenas de texto en memoria física (`<sstream>`).

---

## Progresión de Complejidad

### Nivel Simple: Manipuladores de Formato
C++ permite alterar la presentación del flujo en línea mediante manipuladores como `std::hex` (imprime enteros en base hexadecimal), `std::fixed` (notación decimal fija) o `std::endl` (inserta un salto de línea y fuerza el vaciado del buffer o *flush*).

### Nivel Aplicado: Inicialización Garantizada mediante el truco del Dummy Estático
Como detalla el libro *Secrets of C++ Masters*, la biblioteca iostreams inicializa sus estructuras globales pesadas (`std::cout`, `std::cin`) exactamente una vez.
- Esto se logra mediante un truco estático en el archivo de cabecera: un objeto dummy estático (`static ios_header_init`) cuyo constructor incrementa un contador y realiza la apertura e inicialización del flujo de sistema en la primera unidad de traducción compilada.
- En la destrucción, cuando el contador llega a 0, se vacían y cierran de forma segura todas las salidas abiertas.

### Nivel Complejo: Screams binarios vs de texto
Al trabajar con archivos (`std::fstream`), la bandera `std::ios::binary` evita que el sistema operativo altere de forma invisible caracteres especiales (como convertir el carácter de nueva línea `\n` a retorno de carro `\r\n` en Windows), lo cual es crítico al leer estructuras binarias compiladas o imágenes.

---

## Diseño de Sistemas
En el diseño de arquitecturas de telemetría y logging, los Screams permiten desacoplar el origen de los logs de su destino final. Un sistema de logging acepta referencias abstractas `std::ostream&`, permitiendo enviar la salida a archivos en disco, a sockets TCP de red o a consola interactiva sin modificar una sola línea de código lógico.

---

## Ejercicios

### Ejercicio 1 — Sobrecargar el operador de extracción `>>`
Crea una clase `Punto` con atributos `x` e `y`. Sobrecarga el operador no miembro `>>` para leer las coordenadas desde la consola. El usuario debe ingresar los datos en formato `x y` (dos enteros separados por espacio). Pruébalo leyendo un punto desde `std::cin`.

---

## Errores Comunes y Anti-Patrones
- **No verificar el estado del flujo tras lectura:** Asumir que la lectura fue exitosa sin verificar `if (stream >> variable)`. Esto causa bucles infinitos si la consola recibe caracteres no numéricos inesperados.
- **Uso excesivo de `std::endl`:** Escribir `std::endl` en lugar del carácter `'\n'` vacía el buffer físico en disco en cada iteración, ralentizando la velocidad de escritura de archivos hasta en un 900%.

---

## Conclusión y Checklist Mental
- [ ] ¿Por qué los operadores `<<` y `>>` para clases de usuario deben ser funciones no miembro?
- [ ] ¿Qué diferencia funcional existe entre `std::endl` y el carácter literal `'\n'`?
- [ ] ¿Cómo garantiza el "truco del dummy estático" la inicialización única de los flujos de sistema globales?
- [ ] ¿Para qué sirve la bandera `std::ios::binary` al trabajar con archivos?

---

*Siguiente tema sugerido: [34 — extern y Linkage en C++](<34 — extern y Linkage en C++.md>)*
