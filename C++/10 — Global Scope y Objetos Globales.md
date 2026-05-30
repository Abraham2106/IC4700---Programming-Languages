# Global Scope y Objetos Globales — Peligros del estado mutable compartido y el problema del orden de inicialización

Los objetos globales introducen dependencias ocultas y problemas severos de orden de inicialización entre unidades de traducción independientes. Controlar su ciclo de vida y evitar el estado mutable compartido es crítico para la concurrencia y la predictibilidad del software.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
Las variables u objetos declarados fuera de cualquier función, clase o espacio de nombres pertenecen al **ámbito global**. Son visibles por cualquier parte del código y se inicializan al arrancar la aplicación, destruyéndose al finalizar `main`.

### 1.2 ¿Por qué importa?
- **Orden indefinido:** El orden en que se inicializan los objetos globales declarados en diferentes archivos de código fuente (.cpp) no está garantizado por el estándar de C++. Esto puede causar que un objeto global intente usar a otro que aún no ha sido construido, resultando en crashes inexplicables.
- **Concurrencia:** Modificar datos globales compartidos desde múltiples hilos sin sincronización explícita desata condiciones de carrera (data races).

---

## 2. Hook Example

```cpp
// Archivo: logger.h
#pragma once
#include <iostream>
#include <string>

struct Logger {
    Logger() { std::cout << "Logger inicializado.\n"; }
    void log(const std::string& msg) { std::cout << msg << "\n"; }
};

// Declaración global: le dice a otros archivos que este objeto existe en algún lado
extern Logger logger_global;
```

```cpp
// Archivo: logger.cpp
#include "logger.h"
// Definición física de la variable global
Logger logger_global;
```

```cpp
// Archivo: main.cpp
#include "logger.h"

int main() {
    logger_global.log("Mensaje de prueba.");
    return 0;
}
```

---

## 3. Descomposición Under the Hood

### 3.1 Carga del Ejecutable y Fases de Inicialización
Antes de que la primera línea de `main()` se ejecute:
1. **Inicialización Estática:** El cargador del sistema operativo mapea los datos globales simples (por ejemplo, enteros globales) directamente desde el archivo ejecutable a los segmentos de memoria RAM `.data` o `.bss`.
2. **Inicialización Dinámica (Constructores de Objetos):** El compilador genera una lista oculta de llamadas a los constructores de todos los objetos globales complejos. El runtime ejecuta estas llamadas una a una antes de entrar en `main`.
3. **Destrucción:** El runtime registra automáticamente las funciones destructoras usando la llamada del sistema `atexit`, ejecutándolas en orden inverso de construcción al salir de `main`.

---

## 4. Teoría: Ciclo de Vida Global y el Orden Fiasco

### 4.1 1. El Fiasco del Orden de Inicialización Estática
Si tienes:
- Objeto `Logger logger_global;` en `logger.cpp`.
- Objeto `BaseDatos bd_global;` en `db.cpp` que intenta usar `logger_global` en su constructor.

C++ no define qué constructor se llama primero (`logger_global` o `bd_global`). Si `bd_global` se ejecuta primero, intentará acceder a un logger no inicializado, resultando en comportamiento indefinido (UB) o crash.

### 4.2 2. Solución: El patrón Singleton con variable estática local
Este patrón garantiza que la construcción ocurra exactamente en el primer momento en que se solicita el objeto, resolviendo el orden de inicialización:
```cpp
Logger& obtener_logger() {
    static Logger logger; // Construcción garantizada en el primer uso. Segura para hilos.
    return logger;
}
```

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Uso de `extern`
`extern` le avisa al compilador que la variable existe física y únicamente en otro archivo objeto compilado, dejando la resolución de la dirección final en manos del enlazador (linker).
```cpp
extern int nivel_dificultad; // Declaración
```

### 5.2 Nivel Aplicado: Variables globales inmutables
Si los datos globales son constantes, se prefiere declararlos dentro de namespaces usando `constexpr` para que el compilador resuelva sus valores en tiempo de compilación y los optimice.
```cpp
namespace Config {
    constexpr double gravedad = 9.80665;
}
```

### 5.3 Nivel Complejo: `std::call_once` y Lazy Initialization Manual
Si se requiere construir recursos pesados globales de forma segura para hilos en una fase tardía.
```cpp
#include <mutex>

class ConfiguracionPesada {
public:
    void cargar() {}
};

ConfiguracionPesada* instancia = nullptr;
std::once_flag bandera_inicializacion;

void inicializar_config() {
    instancia = new ConfiguracionPesada();
    instancia->cargar();
}

void acceder_config() {
    std::call_once(bandera_inicializacion, inicializar_config);
    // instancia->usar();
}
```

---

## 6. Diseño de Sistemas
En arquitecturas modernas de software limpio, las variables globales modificables son consideradas un anti-patrón de diseño crítico. En su lugar, se prefiere la **Inyección de Dependencias** (pasar las instancias necesarias por parámetro de constructor) para facilitar la modularidad y las pruebas unitarias.

---

## Exercises

### Exercise 1 — Resolver el Fiasco de Inicialización Estática
Refactoriza la clase de base de datos simulada para evitar el fiasco de inicialización estática utilizando una función que devuelva una referencia a una variable local estática.

```cpp
#include <iostream>

struct Logger {
    Logger() { std::cout << "Logger listo.\n"; }
    void log(const std::string& msg) { std::cout << msg << "\n"; }
};

// Objeto global propenso a fallar si otro objeto se construye antes que él
Logger logger;

struct BaseDatos {
    BaseDatos() {
        // ERROR POTENCIAL: ¿Qué pasa si 'logger' aún no se ha construido?
        logger.log("Base de datos conectando..."); 
    }
};

// TODO: Refactorizar 'logger' y 'BaseDatos' para utilizar el patrón de variable local estática (Singleton)

BaseDatos bd; // Global propenso a fallar

int main() {
    std::cout << "Ejecutando main.\n";
    return 0;
}
```

---

## 7. Errores Comunes y Anti-Patrones
- **Usar variables globales para transferir datos entre funciones:** Dificulta el rastreo del flujo de datos y destruye la reusabilidad del código.
- **No sincronizar accesos globales:** Suponer que leer y escribir una variable global simple (como un entero) es atómico en sistemas multihilo. Siempre se debe proteger con `std::atomic` o `std::mutex`.

---

## 8. Conclusión

---

---

*Next: `11 — Sobrecarga (Overloading).md` — Resolución de sobrecarga y name mangling.*
