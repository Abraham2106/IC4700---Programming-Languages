# 10 — Global Scope y Objetos Globales

> **Resumen Ejecutivo:** Las variables y objetos globales en C++ residen en el ámbito global del programa y tienen un ciclo de vida estático. Aunque facilitan el acceso a datos compartidos, introducen graves problemas de acoplamiento, riesgos en entornos multi-hilo, y el temido "Fiasco del Orden de Inicialización Estática".
>
> **Prerrequisitos:** Haber leído [07 — Ámbito y Ciclo de Vida (Scope)](07 — Funciones y Operadores de Acceso.md).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Ciclo de Vida Global y el Orden Fiasco](#teoría-ciclo-de-vida-global-y-el-orden-fiasco)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
Las variables u objetos declarados fuera de cualquier función, clase o espacio de nombres pertenecen al **ámbito global**. Son visibles por cualquier parte del código y se inicializan al arrancar la aplicación, destruyéndose al finalizar `main`.

### ¿Por qué importa?
- **Orden indefinido:** El orden en que se inicializan los objetos globales declarados en diferentes archivos de código fuente (.cpp) no está garantizado por el estándar de C++. Esto puede causar que un objeto global intente usar a otro que aún no ha sido construido, resultando en crashes inexplicables.
- **Concurrencia:** Modificar datos globales compartidos desde múltiples hilos sin sincronización explícita desata condiciones de carrera (data races).

---

## Conceptos Previos
- Comprensión de la compilación por separado (archivos .cpp independientes).
- Qué es la palabra clave `extern`.

---

## Hook Example

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

## Descomposición Under the Hood

### Carga del Ejecutable y Fases de Inicialización
Antes de que la primera línea de `main()` se ejecute:
1. **Inicialización Estática:** El cargador del sistema operativo mapea los datos globales simples (por ejemplo, enteros globales) directamente desde el archivo ejecutable a los segmentos de memoria RAM `.data` o `.bss`.
2. **Inicialización Dinámica (Constructores de Objetos):** El compilador genera una lista oculta de llamadas a los constructores de todos los objetos globales complejos. El runtime ejecuta estas llamadas una a una antes de entrar en `main`.
3. **Destrucción:** El runtime registra automáticamente las funciones destructoras usando la llamada del sistema `atexit`, ejecutándolas en orden inverso de construcción al salir de `main`.

---

## Teoría: Ciclo de Vida Global y el Orden Fiasco

### 1. El Fiasco del Orden de Inicialización Estática
Si tienes:
- Objeto `Logger logger_global;` en `logger.cpp`.
- Objeto `BaseDatos bd_global;` en `db.cpp` que intenta usar `logger_global` en su constructor.

C++ no define qué constructor se llama primero (`logger_global` o `bd_global`). Si `bd_global` se ejecuta primero, intentará acceder a un logger no inicializado, resultando en comportamiento indefinido (UB) o crash.

### 2. Solución: El patrón Singleton con variable estática local
Este patrón garantiza que la construcción ocurra exactamente en el primer momento en que se solicita el objeto, resolviendo el orden de inicialización:
```cpp
Logger& obtener_logger() {
    static Logger logger; // Construcción garantizada en el primer uso. Segura para hilos.
    return logger;
}
```

---

## Progresión de Complejidad

### Nivel Simple: Uso de `extern`
`extern` le avisa al compilador que la variable existe física y únicamente en otro archivo objeto compilado, dejando la resolución de la dirección final en manos del enlazador (linker).
```cpp
extern int nivel_dificultad; // Declaración
```

### Nivel Aplicado: Variables globales inmutables
Si los datos globales son constantes, se prefiere declararlos dentro de namespaces usando `constexpr` para que el compilador resuelva sus valores en tiempo de compilación y los optimice.
```cpp
namespace Config {
    constexpr double gravedad = 9.80665;
}
```

### Nivel Complejo: `std::call_once` y Lazy Initialization Manual
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

## Diseño de Sistemas
En arquitecturas modernas de software limpio, las variables globales modificables son consideradas un anti-patrón de diseño crítico. En su lugar, se prefiere la **Inyección de Dependencias** (pasar las instancias necesarias por parámetro de constructor) para facilitar la modularidad y las pruebas unitarias.

---

## Ejercicios

### Ejercicio 1 — Resolver el Fiasco de Inicialización Estática
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

## Errores Comunes y Anti-Patrones
- **Usar variables globales para transferir datos entre funciones:** Dificulta el rastreo del flujo de datos y destruye la reusabilidad del código.
- **No sincronizar accesos globales:** Suponer que leer y escribir una variable global simple (como un entero) es atómico en sistemas multihilo. Siempre se debe proteger con `std::atomic` o `std::mutex`.

---

## Conclusión y Checklist Mental
- [ ] ¿Puedes explicar qué es el Fiasco del Orden de Inicialización Estática?
- [ ] ¿Cómo soluciona una variable estática local el problema del orden de construcción?
- [ ] ¿Cuál es la diferencia entre declarar una variable con `extern` y definirla físicamente?

---

*Siguiente tema sugerido: [11 — Sobrecarga (Overloading)](11 — Sobrecarga (Overloading).md)*





