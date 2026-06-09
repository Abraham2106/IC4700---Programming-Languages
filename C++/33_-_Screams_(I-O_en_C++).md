# Streams en C++ — El flujo extensible de entrada y salida estándar y formateadores

La biblioteca de streams en C++ organiza la entrada y salida de datos a través de una jerarquía de clases con sobrecarga de operadores de inserción y extracción. Esta abstracción proporciona un mecanismo type-safe e independiente del dispositivo para flujos de datos.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
Un Scream (stream) es una abstracción lógica que representa un flujo secuencial de datos de un origen (source) a un destino (sink). C++ encapsula los flujos de entrada en la clase `std::istream` y los de salida en `std::ostream`.

### 1.2 ¿Por qué importa?
- **Seguridad Estática:** El compilador deduce el tipo de dato que se inserta en el flujo de forma estática. Se evitan fallos de segmentación por discrepancia de formato en tiempo de ejecución (como pasar un entero cuando `printf` esperaba `%s`).
- **Encadenamiento (Chaining):** Los operadores retornan una referencia al propio objeto Scream (`std::ostream&`), lo que permite escribir expresiones fluidas continuas como `std::cout << x << y << z;`.

---

## 2. Hook Example

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

## 3. Descomposición Under the Hood

### 3.1 ¿Cómo funciona el buffer de un Scream?
Las clases de Screams son wrappers de formateo de alto nivel. La gestión de buffers de memoria física subyacente la realiza la clase `std::streambuf`.
- Cuando escribes `std::cout << 100;`, el formateador convierte el número entero en la secuencia de caracteres `'1'`, `'0'`, `'0'` y los escribe en el buffer interno del flujo.
- **Sincronización:** Para mejorar el rendimiento, C++ sincroniza por defecto sus Screams con las funciones clásicas de C (`stdio`), lo que puede ralentizar operaciones de consola masivas. Se puede desactivar mediante `std::ios_base::sync_with_stdio(false);`.

---

## 4. Teoría: La Jerarquía de Screams en C++

La biblioteca estándar de C++ organiza los Screams en una jerarquía orientada a objetos:

1. **`std::ios_base` y `std::ios`:** Definen las banderas de estado del flujo (ej. `eof()`, `fail()`, `bad()`) y manipuladores de formato.
2. **`std::istream` y `std::ostream`:** Clases base para operaciones de lectura (`>>`, `get()`, `read()`) y escritura (`<<`, `put()`, `write()`).
3. **`std::ifstream` / `std::ofstream`:** Flujos especializados para lectura y escritura de archivos en disco (`<fstream>`).
4. **`std::stringstream`:** Flujos especializados para procesar cadenas de texto en memoria física (`<sstream>`).

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Manipuladores de Formato
C++ permite alterar la presentación del flujo en línea mediante manipuladores como `std::hex` (imprime enteros en base hexadecimal), `std::fixed` (notación decimal fija) o `std::endl` (inserta un salto de línea y fuerza el vaciado del buffer o *flush*).

### 5.2 Nivel Aplicado: Inicialización Garantizada mediante el truco del Dummy Estático
Como detalla el libro *Secrets of C++ Masters*, la biblioteca iostreams inicializa sus estructuras globales pesadas (`std::cout`, `std::cin`) exactamente una vez.
- Esto se logra mediante un truco estático en el archivo de cabecera: un objeto dummy estático (`static ios_header_init`) cuyo constructor incrementa un contador y realiza la apertura e inicialización del flujo de sistema en la primera unidad de traducción compilada.
- En la destrucción, cuando el contador llega a 0, se vacían y cierran de forma segura todas las salidas abiertas.

### 5.3 Nivel Complejo: Screams binarios vs de texto
Al trabajar con archivos (`std::fstream`), la bandera `std::ios::binary` evita que el sistema operativo altere de forma invisible caracteres especiales (como convertir el carácter de nueva línea `\n` a retorno de carro `\r\n` en Windows), lo cual es crítico al leer estructuras binarias compiladas o imágenes.

---

## 6. Diseño de Sistemas
En el diseño de arquitecturas de telemetría y logging, los Screams permiten desacoplar el origen de los logs de su destino final. Un sistema de logging acepta referencias abstractas `std::ostream&`, permitiendo enviar la salida a archivos en disco, a sockets TCP de red o a consola interactiva sin modificar una sola línea de código lógico.

---

## Exercises

### Exercise 1 — Sobrecargar el operador de extracción `>>`
Crea una clase `Punto` con atributos `x` e `y`. Sobrecarga el operador no miembro `>>` para leer las coordenadas desde la consola. El usuario debe ingresar los datos en formato `x y` (dos enteros separados por espacio). Pruébalo leyendo un punto desde `std::cin`.

---

## 7. Errores Comunes y Anti-Patrones
- **No verificar el estado del flujo tras lectura:** Asumir que la lectura fue exitosa sin verificar `if (stream >> variable)`. Esto causa bucles infinitos si la consola recibe caracteres no numéricos inesperados.
- **Uso excesivo de `std::endl`:** Escribir `std::endl` en lugar del carácter `'\n'` vacía el buffer físico en disco en cada iteración, ralentizando la velocidad de escritura de archivos hasta en un 900%.

---

## 8. Conclusión

---

---

*Next: `34 — extern y Linkage en C++.md` — Visibilidad de símbolos entre unidades de traducción.*
