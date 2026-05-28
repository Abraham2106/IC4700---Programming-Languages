# 01 — Diferencias entre C, C++ y Rust

> **Resumen Ejecutivo:** Tres lenguajes de sistemas, tres filosofías radicalmente distintas sobre quién controla la memoria y cómo se garantiza la seguridad. Entender sus diferencias es entender la evolución del software de bajo nivel.
>
> **Prerrequisitos:** Ninguno estricto. Ayuda haber visto pseudocódigo o algún lenguaje de alto nivel.
> **Clasificación:** TEMA MACRO

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Los Tres Paradigmas](#teoría-los-tres-paradigmas)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?

C, C++ y Rust son tres lenguajes que compiten en el mismo nicho: **código cercano al hardware, máximo rendimiento, control total sobre la memoria**. Sin embargo, cada uno tiene una respuesta distinta a la pregunta: *"¿Quién es responsable de que el programa no se rompa?"*

- **C** responde: *El programador. Siempre. Sin red de seguridad.*
- **C++** responde: *El programador, pero te damos herramientas para ayudarte (si las usas).*
- **Rust** responde: *El compilador. Antes de que el programa corra, él verifica la corrección.*

### ¿Por qué importa?

El 70% de las vulnerabilidades de seguridad en software de sistemas (CVEs de Chrome, Linux, Windows) provienen de errores de memoria: use-after-free, buffer overflows, data races. Estas tres decisiones de diseño lingüístico tienen consecuencias directas en **seguridad nacional, infraestructura crítica y millones de dispositivos IoT**.

### Dónde se usan en la vida real

| Lenguaje | Dominio principal |
|---|---|
| C | Kernels (Linux), firmware, drivers, sistemas embebidos |
| C++ | Game engines (Unreal), navegadores (Chrome/Firefox), bases de datos (MySQL), trading HFT |
| Rust | WebAssembly, async runtimes (Tokio), seguridad (Firefox Quantum, Android HAL) |

### Objetivos de Aprendizaje

Al finalizar este artículo podrás:
- Comprender las diferencias filosóficas entre los tres lenguajes
- Identificar cuándo elegir cada uno según el contexto
- Entender el modelo de ownership de Rust vs el modelo manual de C/C++
- Evitar el error de tratar C++ como "C con clases" (es mucho más)

---

## Conceptos Previos

Antes de comenzar deberías conocer:
- Qué es la memoria RAM y la diferencia entre Stack y Heap (superficialmente)
- Qué es compilar vs interpretar
- Qué es un puntero conceptualmente (una dirección de memoria)

---

## Hook Example

```c
// C: Tú manejas todo. El compilador no te avisa.
#include <stdio.h>
#include <stdlib.h>

int main() {
    int* p = (int*)malloc(sizeof(int));
    *p = 42;
    printf("%d\n", *p);
    free(p);
    printf("%d\n", *p); // USE AFTER FREE — comportamiento indefinido. C no dice nada.
    return 0;
}
```

```cpp
// C++: Tienes herramientas modernas, pero puedes ignorarlas.
#include <memory>
#include <iostream>

int main() {
    auto p = std::make_unique<int>(42);
    std::cout << *p << "\n";
    // p se destruye automáticamente al salir del scope — RAII
}
```

```rust
// Rust: El compilador rechaza el error antes de ejecutar.
fn main() {
    let p = Box::new(42);
    println!("{}", p);
    // Aquí p se libera. No puedes usar p después. El compilador lo prohíbe.
}
```

**¿Qué notas?** El mismo problema conceptual (memoria dinámica + entero) produce tres paradigmas radicalmente diferentes. C te deja cometer el error. C++ te da herramientas para evitarlo. Rust **te prohíbe** cometerlo en tiempo de compilación.

---

## Descomposición Under the Hood

### ¿Qué hace el compilador con cada uno?

**C — `malloc` + `free`:**
- `malloc(sizeof(int))` hace una syscall a `brk` o `mmap` en Linux, reservando bytes en el heap.
- El compilador de C **no trackea** quién es el dueño de esa memoria.
- `free(p)` devuelve el bloque al allocator. `p` sigue apuntando a la misma dirección (dangling pointer).
- La línea `printf("%d\n", *p)` después del `free` es **Undefined Behavior (UB)**. El programa puede imprimir 42, crashear, o ejecutar código malicioso. El estándar dice literalmente "anything can happen".

**C++ — `std::unique_ptr` (RAII):**
- `make_unique<int>(42)` también llama a `new` (que internamente usa `malloc`-equivalente).
- `unique_ptr` es un **wrapper** (clase con destructor). Al salir del scope, su destructor llama `delete` automáticamente.
- El compilador **sí trackea** el scope del objeto. Esto es RAII (Resource Acquisition Is Initialization).
- Si intentas copiar un `unique_ptr`, el compilador falla en compilación (está deleted).

**Rust — Ownership System:**
- `Box::new(42)` aloca en el heap, pero el compilador asigna un **Owner** único: la variable `p`.
- El compilador tiene un componente llamado **Borrow Checker** que verifica en compile-time que:
  1. Cada valor tiene exactamente un dueño.
  2. El dueño libera la memoria al salir de scope (igual que RAII).
  3. No puedes usar un valor después de que fue movido o liberado.
- No existe garbage collector. No existe runtime overhead. La seguridad es **cero-costo**.

### Tabla de modelo de memoria

| Aspecto | C | C++ | Rust |
|---|---|---|---|
| Gestión de memoria | Manual (malloc/free) | Manual o RAII | Ownership + Borrow Checker |
| Cuándo se detectan errores de memoria | Runtime (o nunca) | Runtime (sin RAII) / Compile-time (con RAII) | Compile-time siempre |
| Garbage Collector | No | No | No |
| Null pointers | Sí (`NULL`) | Sí (`nullptr`) | No (usa `Option<T>`) |
| Data races | Posibles | Posibles | Imposibles (garantía del compilador) |
| Herencia OOP | No | Sí | No (usa Traits) |
| Abstracción cero-costo | Sí (no hay abstracciones) | Sí (con inline, templates) | Sí (garantía del lenguaje) |

---

## Teoría: Los Tres Paradigmas

### C — El Lenguaje de la Máquina

C fue diseñado en 1972 por Dennis Ritchie para reescribir Unix. Su filosofía es **"confía en el programador"**. El lenguaje es casi una abstracción delgada sobre el ensamblador.

**Características clave:**
- Tipado estático, pero débil (conversiones implícitas peligrosas)
- Sin clases ni objetos (estructural, no orientado a objetos)
- Sin namespaces nativos
- Sin manejo de excepciones
- Sin templates/genéricos
- `struct` sin métodos
- Todo es responsabilidad del programador

**Cuándo usar C:**
- Kernels de SO (necesitas control total del hardware)
- Firmware (microcontroladores con 2KB de RAM)
- Drivers de dispositivos
- Cuando el runtime de C++ es inaceptable

### C++ — El Lenguaje de las Abstracciones de Cero Costo

Bjarne Stroustrup diseñó C++ en los 80s con una premisa: **añadir OOP y abstracciones poderosas sin pagar costo en runtime**. C++ es un superconjunto casi completo de C.

**Características clave sobre C:**
- Clases, herencia, polimorfismo virtual
- Templates (metaprogramación en compile-time)
- RAII (destructores garantizados)
- Excepciones (costosas, pero existen)
- `std::` library: STL, algoritmos, contenedores
- Referencias (alias seguros de punteros)
- Sobrecarga de operadores
- `constexpr` (evaluación en compile-time)
- Move semantics (C++11) — elimina copias innecesarias

**Error conceptual grave:** Tratar C++ como "C con clases" ignora 40 años de evolución. C++ moderno (C++17/20) con `std::variant`, ranges, concepts y coroutines es casi irreconocible comparado con C.

### Rust — El Lenguaje de la Seguridad sin GC

Rust (Mozilla, 2010) nació de una pregunta: *¿Es posible tener la velocidad de C/C++ con garantías de seguridad de memoria en compile-time, sin garbage collector?* La respuesta fue el **Ownership System**.

**Características clave:**
- Ownership + Borrow Checker (única innovación central)
- Sin clases — usa `struct` + `impl` + `trait` (similar a interfaces)
- Sin herencia de implementación (solo de interfaz via Traits)
- `Option<T>` en lugar de null — fuerza manejo explícito de ausencia de valor
- `Result<T, E>` en lugar de excepciones — fuerza manejo explícito de errores
- Inmutabilidad por defecto (`let x = 5` es inmutable; `let mut x = 5` es mutable)
- Pattern matching exhaustivo
- Fearless concurrency — data races son un error de compilación

---

## Progresión de Complejidad

### Nivel Simple: La misma función en los tres lenguajes

```c
// C: Suma de array
int sum(int* arr, int len) {
    int total = 0;
    for (int i = 0; i < len; i++) total += arr[i];
    return total;
}
```

```cpp
// C++: Usando templates y rangos
#include <numeric>
#include <vector>

template<typename Container>
auto sum(const Container& c) {
    return std::accumulate(c.begin(), c.end(), 0);
}
```

```rust
// Rust: Iteradores funcionales, cero overhead
fn sum(arr: &[i32]) -> i32 {
    arr.iter().sum()
}
```

### Nivel Aplicado: Manejo de errores

```c
// C: Retorna -1 para indicar error. Convención frágil.
int read_file(const char* path, char* buffer, int size) {
    FILE* f = fopen(path, "r");
    if (!f) return -1;
    fread(buffer, 1, size, f);
    fclose(f);
    return 0;
}
```

```cpp
// C++: Excepciones — poderosas pero costosas y no visibles en la firma
#include <fstream>
#include <stdexcept>
#include <string>

std::string read_file(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("No se pudo abrir: " + path);
    return {std::istreambuf_iterator<char>(f), {}};
}
```

```rust
// Rust: Result<T,E> — el error ES parte de la firma. No puedes ignorarlo.
use std::fs;
use std::io;

fn read_file(path: &str) -> Result<String, io::Error> {
    fs::read_to_string(path)
}
// El llamador DEBE hacer match o propagar con ? — imposible ignorar el error.
```

### Nivel Complejo: Concurrencia

```cpp
// C++: Data race posible. El compilador no lo detecta.
#include <thread>
int contador = 0; // Variable global compartida

void incrementar() {
    for (int i = 0; i < 1000000; i++) contador++; // RACE CONDITION
}

int main() {
    std::thread t1(incrementar), t2(incrementar);
    t1.join(); t2.join();
    // contador NO será 2000000. El resultado es impredecible.
}
```

```rust
// Rust: El compilador rechaza el data race antes de compilar.
use std::sync::{Arc, Mutex};
use std::thread;

fn main() {
    let contador = Arc::new(Mutex::new(0));
    let mut handles = vec![];

    for _ in 0..2 {
        let c = Arc::clone(&contador);
        handles.push(thread::spawn(move || {
            let mut num = c.lock().unwrap();
            *num += 1000000;
        }));
    }
    for h in handles { h.join().unwrap(); }
    // Garantizado: resultado es siempre 2000000.
}
```

---

## Diseño de Sistemas

### ¿Cuándo elegir qué en un sistema real?

**Escenario 1: Kernel de SO / Driver**
→ C. Sin runtime, sin excepciones, control absoluto del hardware. Rust está ganando terreno (Linux 6.1 acepta Rust en drivers).

**Escenario 2: Game Engine (AAA)**
→ C++. El ecosistema de herramientas (profilers, debuggers, middleware) está construido para C++. Unreal Engine es C++. Move semantics eliminan copias en hot paths.

**Escenario 3: Servidor Web de alta concurrencia**
→ Rust (Actix-web, Axum). Benchmarks muestran rendimiento comparable a C++ con seguridad de memoria garantizada. Tokio (async runtime) maneja millones de conexiones.

**Escenario 4: Sistema embebido con restricciones de memoria**
→ C o Rust. Rust tiene un perfil `no_std` para entornos sin sistema operativo.

**Escenario 5: Base de datos (ej. nuevo motor)**
→ C++ o Rust. RocksDB (C++), TiKV (Rust).

### CAP / Latencia

- Los tres lenguajes son **deterministas** (sin GC pauses). Apropiados para sistemas de baja latencia.
- C++ con `std::allocator` custom puede lograr latencias de microsegundos en HFT.
- Rust elimina la categoría entera de bugs de memoria que causan latencias impredecibles por crashes y reinicios.

---

## Proyecto Aplicado

### Mini-proyecto: Procesador de logs de seguridad

**Escenario:** Un sistema de detección de intrusiones analiza 500,000 líneas de log por segundo. Necesita: velocidad máxima, cero crashes por buffer overflow, soporte de concurrencia.

**Decisión arquitectónica:**

```
C   → Descartado: demasiado riesgo de CVEs por buffer overflows en parsing.
C++ → Viable: con RAII y smart pointers. Riesgo residual de data races.
Rust → Elegido: Borrow Checker elimina las dos categorías de riesgo.
```

**Métricas simuladas:**
- Throughput: 2.1M líneas/segundo en Rust vs 1.9M en C++ (diferencia de ~10% por overhead de mutex validation en C++)
- CVEs introducidos en 6 meses de desarrollo: C++ legacy = 4 bugs de memoria, Rust = 0
- Tiempo de debug de concurrencia: C++ = 3 semanas por data race sutil, Rust = detectado en compile-time en día 1

**Documentación Técnica:** `ARCHITECTURE.md` — decision log: "Elegimos Rust sobre C++ debido a las garantías de compile-time del Borrow Checker para un sistema donde un crash en producción implica una brecha de seguridad no detectada."

---

## Ejercicios

### Ejercicio 1 — Comparación de gestión de memoria

**Objetivo:** Entender empíricamente la diferencia entre gestión manual y RAII.

```cpp
// TODO: En C++, implementa una clase "Buffer" que:
// 1. En su constructor, aloque un array de ints en el heap con `new`
// 2. En su destructor, libere esa memoria con `delete[]`
// 3. En main(), crea un Buffer dentro de un if-scope y observa cuándo se destruye

class Buffer {
    int* data;
    int size;
public:
    Buffer(int n) {
        // TODO: Implementar construcción
    }
    ~Buffer() {
        // TODO: Implementar destrucción
    }
};

int main() {
    if (true) {
        Buffer b(100);
        // TODO: Agregar print en constructor y destructor para observar el ciclo de vida
    }
    // ¿Cuándo se imprimió el destructor? ¿Por qué?
    return 0;
}
```

**Edge case a considerar:** ¿Qué pasa si lanzas una excepción dentro del scope? ¿Se llama el destructor?

### Ejercicio 2 — El costo del UB

**Objetivo:** Observar comportamiento indefinido en C.

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int* arr = (int*)malloc(5 * sizeof(int));
    for (int i = 0; i < 5; i++) arr[i] = i * 10;
    
    free(arr);
    
    // TODO: Intenta acceder a arr[0] después del free.
    // Compila con: gcc -O0 y con gcc -O2
    // ¿Los resultados son iguales? ¿Por qué no?
    // Documenta tu observación.
    
    return 0;
}
```

### Ejercicio 3 — Análisis de trade-offs

**Objetivo:** Razonamiento arquitectónico, no código.

Dado el siguiente sistema: un parser de paquetes de red que corre en un router industrial con 512KB de RAM, latencia máxima de 1ms, y que debe ser auditado por seguridad cada año.

Escribe un documento de decisión de 10 líneas justificando tu elección entre C, C++ y Rust. Considera: tamaño del equipo (2 personas), vida útil del producto (10 años), y que el código procesará datos no confiables de internet.

---

## Errores Comunes y Anti-Patrones

### Error #1: Tratar C++ como C

```cpp
// Malo — C++ escrito como C
#include <stdio.h>
#include <stdlib.h>

int main() {
    int* datos = (int*)malloc(100 * sizeof(int));
    // ... usar datos ...
    free(datos);
    return 0;
}
```

```cpp
// Bueno — C++ moderno
#include <vector>

int main() {
    std::vector<int> datos(100);
    // ... usar datos ...
    // Liberación automática garantizada
    return 0;
}
```

**Problema:** `malloc` en C++ bypasea constructores. Un `int` está bien, pero `malloc` de objetos C++ produce UB porque no llama constructores.

### Error #2: Pensar que Rust es "C++ más seguro"

Rust no tiene herencia. No tiene clases. No tiene null. No tiene excepciones implícitas. Es un lenguaje diferente con un modelo de tipos diferente. El error es intentar traducir código C++ a Rust línea a línea. Rust requiere pensar en ownership desde el diseño.

### Error #3: Usar `using namespace std` en headers de C++

```cpp
// Malo — en un .h contamina todos los que incluyan este header
#pragma once
using namespace std; // Esto es un error grave en headers
string get_name();
```

```cpp
// Bueno — califica explícitamente
#pragma once
std::string get_name();
```

---

## Conclusión y Checklist Mental

**Los tres puntos más críticos:**
1. La diferencia fundamental no es sintaxis — es **quién detecta los errores de memoria**: el programador (C), las convenciones del programador (C++), o el compilador (Rust).
2. C++ moderno (post-C++11) con RAII, smart pointers y STL es un lenguaje diferente al C++ de los 90s. No juzgues C++ por código legacy.
3. Rust no reemplaza C++ en todos los contextos — el ecosistema, las herramientas y el talento disponible son factores reales de ingeniería.

**Checklist de retención:**
- [ ] ¿Puedes explicar qué es el Borrow Checker de Rust y qué categoría de bugs elimina?
- [ ] ¿Puedes decir por qué `malloc` en C++ para objetos es Undefined Behavior?
- [ ] ¿Puedes dar un ejemplo de un sistema donde C sería la elección correcta sobre Rust?
- [ ] ¿Sabes qué es RAII y en qué lenguaje(s) aplica?
- [ ] ¿Puedes explicar por qué un data race es un error de compilación en Rust pero no en C++?

---

*Siguiente tema sugerido: `02_variables_y_tipos.md` — el sistema de tipos de C++ y cómo difiere de C.*



