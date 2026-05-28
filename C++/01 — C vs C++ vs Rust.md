# 01 â€” Diferencias entre C, C++ y Rust

> **Resumen Ejecutivo:** Tres lenguajes de sistemas, tres filosofÃ­as radicalmente distintas sobre quiÃ©n controla la memoria y cÃ³mo se garantiza la seguridad. Entender sus diferencias es entender la evoluciÃ³n del software de bajo nivel.
>
> **Prerrequisitos:** Ninguno estricto. Ayuda haber visto pseudocÃ³digo o algÃºn lenguaje de alto nivel.
> **ClasificaciÃ³n:** TEMA MACRO

---

## Tabla de Contenidos

- [IntroducciÃ³n](#introducciÃ³n)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [DescomposiciÃ³n Under the Hood](#descomposiciÃ³n-under-the-hood)
- [TeorÃ­a: Los Tres Paradigmas](#teorÃ­a-los-tres-paradigmas)
- [ProgresiÃ³n de Complejidad](#progresiÃ³n-de-complejidad)
- [DiseÃ±o de Sistemas](#diseÃ±o-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [ConclusiÃ³n y Checklist Mental](#conclusiÃ³n-y-checklist-mental)

---

## IntroducciÃ³n

### Â¿QuÃ© es este tema?

C, C++ y Rust son tres lenguajes que compiten en el mismo nicho: **cÃ³digo cercano al hardware, mÃ¡ximo rendimiento, control total sobre la memoria**. Sin embargo, cada uno tiene una respuesta distinta a la pregunta: *"Â¿QuiÃ©n es responsable de que el programa no se rompa?"*

- **C** responde: *El programador. Siempre. Sin red de seguridad.*
- **C++** responde: *El programador, pero te damos herramientas para ayudarte (si las usas).*
- **Rust** responde: *El compilador. Antes de que el programa corra, Ã©l verifica la correcciÃ³n.*

### Â¿Por quÃ© importa?

El 70% de las vulnerabilidades de seguridad en software de sistemas (CVEs de Chrome, Linux, Windows) provienen de errores de memoria: use-after-free, buffer overflows, data races. Estas tres decisiones de diseÃ±o lingÃ¼Ã­stico tienen consecuencias directas en **seguridad nacional, infraestructura crÃ­tica y millones de dispositivos IoT**.

### DÃ³nde se usan en la vida real

| Lenguaje | Dominio principal |
|---|---|
| C | Kernels (Linux), firmware, drivers, sistemas embebidos |
| C++ | Game engines (Unreal), navegadores (Chrome/Firefox), bases de datos (MySQL), trading HFT |
| Rust | WebAssembly, async runtimes (Tokio), seguridad (Firefox Quantum, Android HAL) |

### Objetivos de Aprendizaje

Al finalizar este artÃ­culo podrÃ¡s:
- Comprender las diferencias filosÃ³ficas entre los tres lenguajes
- Identificar cuÃ¡ndo elegir cada uno segÃºn el contexto
- Entender el modelo de ownership de Rust vs el modelo manual de C/C++
- Evitar el error de tratar C++ como "C con clases" (es mucho mÃ¡s)

---

## Conceptos Previos

Antes de comenzar deberÃ­as conocer:
- QuÃ© es la memoria RAM y la diferencia entre Stack y Heap (superficialmente)
- QuÃ© es compilar vs interpretar
- QuÃ© es un puntero conceptualmente (una direcciÃ³n de memoria)

---

## Hook Example

```c
// C: TÃº manejas todo. El compilador no te avisa.
#include <stdio.h>
#include <stdlib.h>

int main() {
    int* p = (int*)malloc(sizeof(int));
    *p = 42;
    printf("%d\n", *p);
    free(p);
    printf("%d\n", *p); // USE AFTER FREE â€” comportamiento indefinido. C no dice nada.
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
    // p se destruye automÃ¡ticamente al salir del scope â€” RAII
}
```

```rust
// Rust: El compilador rechaza el error antes de ejecutar.
fn main() {
    let p = Box::new(42);
    println!("{}", p);
    // AquÃ­ p se libera. No puedes usar p despuÃ©s. El compilador lo prohÃ­be.
}
```

**Â¿QuÃ© notas?** El mismo problema conceptual (memoria dinÃ¡mica + entero) produce tres paradigmas radicalmente diferentes. C te deja cometer el error. C++ te da herramientas para evitarlo. Rust **te prohÃ­be** cometerlo en tiempo de compilaciÃ³n.

---

## DescomposiciÃ³n Under the Hood

### Â¿QuÃ© hace el compilador con cada uno?

**C â€” `malloc` + `free`:**
- `malloc(sizeof(int))` hace una syscall a `brk` o `mmap` en Linux, reservando bytes en el heap.
- El compilador de C **no trackea** quiÃ©n es el dueÃ±o de esa memoria.
- `free(p)` devuelve el bloque al allocator. `p` sigue apuntando a la misma direcciÃ³n (dangling pointer).
- La lÃ­nea `printf("%d\n", *p)` despuÃ©s del `free` es **Undefined Behavior (UB)**. El programa puede imprimir 42, crashear, o ejecutar cÃ³digo malicioso. El estÃ¡ndar dice literalmente "anything can happen".

**C++ â€” `std::unique_ptr` (RAII):**
- `make_unique<int>(42)` tambiÃ©n llama a `new` (que internamente usa `malloc`-equivalente).
- `unique_ptr` es un **wrapper** (clase con destructor). Al salir del scope, su destructor llama `delete` automÃ¡ticamente.
- El compilador **sÃ­ trackea** el scope del objeto. Esto es RAII (Resource Acquisition Is Initialization).
- Si intentas copiar un `unique_ptr`, el compilador falla en compilaciÃ³n (estÃ¡ deleted).

**Rust â€” Ownership System:**
- `Box::new(42)` aloca en el heap, pero el compilador asigna un **Owner** Ãºnico: la variable `p`.
- El compilador tiene un componente llamado **Borrow Checker** que verifica en compile-time que:
  1. Cada valor tiene exactamente un dueÃ±o.
  2. El dueÃ±o libera la memoria al salir de scope (igual que RAII).
  3. No puedes usar un valor despuÃ©s de que fue movido o liberado.
- No existe garbage collector. No existe runtime overhead. La seguridad es **cero-costo**.

### Tabla de modelo de memoria

| Aspecto | C | C++ | Rust |
|---|---|---|---|
| GestiÃ³n de memoria | Manual (malloc/free) | Manual o RAII | Ownership + Borrow Checker |
| CuÃ¡ndo se detectan errores de memoria | Runtime (o nunca) | Runtime (sin RAII) / Compile-time (con RAII) | Compile-time siempre |
| Garbage Collector | No | No | No |
| Null pointers | SÃ­ (`NULL`) | SÃ­ (`nullptr`) | No (usa `Option<T>`) |
| Data races | Posibles | Posibles | Imposibles (garantÃ­a del compilador) |
| Herencia OOP | No | SÃ­ | No (usa Traits) |
| AbstracciÃ³n cero-costo | SÃ­ (no hay abstracciones) | SÃ­ (con inline, templates) | SÃ­ (garantÃ­a del lenguaje) |

---

## TeorÃ­a: Los Tres Paradigmas

### C â€” El Lenguaje de la MÃ¡quina

C fue diseÃ±ado en 1972 por Dennis Ritchie para reescribir Unix. Su filosofÃ­a es **"confÃ­a en el programador"**. El lenguaje es casi una abstracciÃ³n delgada sobre el ensamblador.

**CaracterÃ­sticas clave:**
- Tipado estÃ¡tico, pero dÃ©bil (conversiones implÃ­citas peligrosas)
- Sin clases ni objetos (estructural, no orientado a objetos)
- Sin namespaces nativos
- Sin manejo de excepciones
- Sin templates/genÃ©ricos
- `struct` sin mÃ©todos
- Todo es responsabilidad del programador

**CuÃ¡ndo usar C:**
- Kernels de SO (necesitas control total del hardware)
- Firmware (microcontroladores con 2KB de RAM)
- Drivers de dispositivos
- Cuando el runtime de C++ es inaceptable

### C++ â€” El Lenguaje de las Abstracciones de Cero Costo

Bjarne Stroustrup diseÃ±Ã³ C++ en los 80s con una premisa: **aÃ±adir OOP y abstracciones poderosas sin pagar costo en runtime**. C++ es un superconjunto casi completo de C.

**CaracterÃ­sticas clave sobre C:**
- Clases, herencia, polimorfismo virtual
- Templates (metaprogramaciÃ³n en compile-time)
- RAII (destructores garantizados)
- Excepciones (costosas, pero existen)
- `std::` library: STL, algoritmos, contenedores
- Referencias (alias seguros de punteros)
- Sobrecarga de operadores
- `constexpr` (evaluaciÃ³n en compile-time)
- Move semantics (C++11) â€” elimina copias innecesarias

**Error conceptual grave:** Tratar C++ como "C con clases" ignora 40 aÃ±os de evoluciÃ³n. C++ moderno (C++17/20) con `std::variant`, ranges, concepts y coroutines es casi irreconocible comparado con C.

### Rust â€” El Lenguaje de la Seguridad sin GC

Rust (Mozilla, 2010) naciÃ³ de una pregunta: *Â¿Es posible tener la velocidad de C/C++ con garantÃ­as de seguridad de memoria en compile-time, sin garbage collector?* La respuesta fue el **Ownership System**.

**CaracterÃ­sticas clave:**
- Ownership + Borrow Checker (Ãºnica innovaciÃ³n central)
- Sin clases â€” usa `struct` + `impl` + `trait` (similar a interfaces)
- Sin herencia de implementaciÃ³n (solo de interfaz via Traits)
- `Option<T>` en lugar de null â€” fuerza manejo explÃ­cito de ausencia de valor
- `Result<T, E>` en lugar de excepciones â€” fuerza manejo explÃ­cito de errores
- Inmutabilidad por defecto (`let x = 5` es inmutable; `let mut x = 5` es mutable)
- Pattern matching exhaustivo
- Fearless concurrency â€” data races son un error de compilaciÃ³n

---

## ProgresiÃ³n de Complejidad

### Nivel Simple: La misma funciÃ³n en los tres lenguajes

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
// C: Retorna -1 para indicar error. ConvenciÃ³n frÃ¡gil.
int read_file(const char* path, char* buffer, int size) {
    FILE* f = fopen(path, "r");
    if (!f) return -1;
    fread(buffer, 1, size, f);
    fclose(f);
    return 0;
}
```

```cpp
// C++: Excepciones â€” poderosas pero costosas y no visibles en la firma
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
// Rust: Result<T,E> â€” el error ES parte de la firma. No puedes ignorarlo.
use std::fs;
use std::io;

fn read_file(path: &str) -> Result<String, io::Error> {
    fs::read_to_string(path)
}
// El llamador DEBE hacer match o propagar con ? â€” imposible ignorar el error.
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
    // contador NO serÃ¡ 2000000. El resultado es impredecible.
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

## DiseÃ±o de Sistemas

### Â¿CuÃ¡ndo elegir quÃ© en un sistema real?

**Escenario 1: Kernel de SO / Driver**
â†’ C. Sin runtime, sin excepciones, control absoluto del hardware. Rust estÃ¡ ganando terreno (Linux 6.1 acepta Rust en drivers).

**Escenario 2: Game Engine (AAA)**
â†’ C++. El ecosistema de herramientas (profilers, debuggers, middleware) estÃ¡ construido para C++. Unreal Engine es C++. Move semantics eliminan copias en hot paths.

**Escenario 3: Servidor Web de alta concurrencia**
â†’ Rust (Actix-web, Axum). Benchmarks muestran rendimiento comparable a C++ con seguridad de memoria garantizada. Tokio (async runtime) maneja millones de conexiones.

**Escenario 4: Sistema embebido con restricciones de memoria**
â†’ C o Rust. Rust tiene un perfil `no_std` para entornos sin sistema operativo.

**Escenario 5: Base de datos (ej. nuevo motor)**
â†’ C++ o Rust. RocksDB (C++), TiKV (Rust).

### CAP / Latencia

- Los tres lenguajes son **deterministas** (sin GC pauses). Apropiados para sistemas de baja latencia.
- C++ con `std::allocator` custom puede lograr latencias de microsegundos en HFT.
- Rust elimina la categorÃ­a entera de bugs de memoria que causan latencias impredecibles por crashes y reinicios.

---

## Proyecto Aplicado

### Mini-proyecto: Procesador de logs de seguridad

**Escenario:** Un sistema de detecciÃ³n de intrusiones analiza 500,000 lÃ­neas de log por segundo. Necesita: velocidad mÃ¡xima, cero crashes por buffer overflow, soporte de concurrencia.

**DecisiÃ³n arquitectÃ³nica:**

```
C   â†’ Descartado: demasiado riesgo de CVEs por buffer overflows en parsing.
C++ â†’ Viable: con RAII y smart pointers. Riesgo residual de data races.
Rust â†’ Elegido: Borrow Checker elimina las dos categorÃ­as de riesgo.
```

**MÃ©tricas simuladas:**
- Throughput: 2.1M lÃ­neas/segundo en Rust vs 1.9M en C++ (diferencia de ~10% por overhead de mutex validation en C++)
- CVEs introducidos en 6 meses de desarrollo: C++ legacy = 4 bugs de memoria, Rust = 0
- Tiempo de debug de concurrencia: C++ = 3 semanas por data race sutil, Rust = detectado en compile-time en dÃ­a 1

**DocumentaciÃ³n TÃ©cnica:** `ARCHITECTURE.md` â€” decision log: "Elegimos Rust sobre C++ debido a las garantÃ­as de compile-time del Borrow Checker para un sistema donde un crash en producciÃ³n implica una brecha de seguridad no detectada."

---

## Ejercicios

### Ejercicio 1 â€” ComparaciÃ³n de gestiÃ³n de memoria

**Objetivo:** Entender empÃ­ricamente la diferencia entre gestiÃ³n manual y RAII.

```cpp
// TODO: En C++, implementa una clase "Buffer" que:
// 1. En su constructor, aloque un array de ints en el heap con `new`
// 2. En su destructor, libere esa memoria con `delete[]`
// 3. En main(), crea un Buffer dentro de un if-scope y observa cuÃ¡ndo se destruye

class Buffer {
    int* data;
    int size;
public:
    Buffer(int n) {
        // TODO: Implementar construcciÃ³n
    }
    ~Buffer() {
        // TODO: Implementar destrucciÃ³n
    }
};

int main() {
    if (true) {
        Buffer b(100);
        // TODO: Agregar print en constructor y destructor para observar el ciclo de vida
    }
    // Â¿CuÃ¡ndo se imprimiÃ³ el destructor? Â¿Por quÃ©?
    return 0;
}
```

**Edge case a considerar:** Â¿QuÃ© pasa si lanzas una excepciÃ³n dentro del scope? Â¿Se llama el destructor?

### Ejercicio 2 â€” El costo del UB

**Objetivo:** Observar comportamiento indefinido en C.

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int* arr = (int*)malloc(5 * sizeof(int));
    for (int i = 0; i < 5; i++) arr[i] = i * 10;
    
    free(arr);
    
    // TODO: Intenta acceder a arr[0] despuÃ©s del free.
    // Compila con: gcc -O0 y con gcc -O2
    // Â¿Los resultados son iguales? Â¿Por quÃ© no?
    // Documenta tu observaciÃ³n.
    
    return 0;
}
```

### Ejercicio 3 â€” AnÃ¡lisis de trade-offs

**Objetivo:** Razonamiento arquitectÃ³nico, no cÃ³digo.

Dado el siguiente sistema: un parser de paquetes de red que corre en un router industrial con 512KB de RAM, latencia mÃ¡xima de 1ms, y que debe ser auditado por seguridad cada aÃ±o.

Escribe un documento de decisiÃ³n de 10 lÃ­neas justificando tu elecciÃ³n entre C, C++ y Rust. Considera: tamaÃ±o del equipo (2 personas), vida Ãºtil del producto (10 aÃ±os), y que el cÃ³digo procesarÃ¡ datos no confiables de internet.

---

## Errores Comunes y Anti-Patrones

### Error #1: Tratar C++ como C

```cpp
// Malo â€” C++ escrito como C
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
// Bueno â€” C++ moderno
#include <vector>

int main() {
    std::vector<int> datos(100);
    // ... usar datos ...
    // LiberaciÃ³n automÃ¡tica garantizada
    return 0;
}
```

**Problema:** `malloc` en C++ bypasea constructores. Un `int` estÃ¡ bien, pero `malloc` de objetos C++ produce UB porque no llama constructores.

### Error #2: Pensar que Rust es "C++ mÃ¡s seguro"

Rust no tiene herencia. No tiene clases. No tiene null. No tiene excepciones implÃ­citas. Es un lenguaje diferente con un modelo de tipos diferente. El error es intentar traducir cÃ³digo C++ a Rust lÃ­nea a lÃ­nea. Rust requiere pensar en ownership desde el diseÃ±o.

### Error #3: Usar `using namespace std` en headers de C++

```cpp
// Malo â€” en un .h contamina todos los que incluyan este header
#pragma once
using namespace std; // Esto es un error grave en headers
string get_name();
```

```cpp
// Bueno â€” califica explÃ­citamente
#pragma once
std::string get_name();
```

---

## ConclusiÃ³n y Checklist Mental

**Los tres puntos mÃ¡s crÃ­ticos:**
1. La diferencia fundamental no es sintaxis â€” es **quiÃ©n detecta los errores de memoria**: el programador (C), las convenciones del programador (C++), o el compilador (Rust).
2. C++ moderno (post-C++11) con RAII, smart pointers y STL es un lenguaje diferente al C++ de los 90s. No juzgues C++ por cÃ³digo legacy.
3. Rust no reemplaza C++ en todos los contextos â€” el ecosistema, las herramientas y el talento disponible son factores reales de ingenierÃ­a.

**Checklist de retenciÃ³n:**
- [ ] Â¿Puedes explicar quÃ© es el Borrow Checker de Rust y quÃ© categorÃ­a de bugs elimina?
- [ ] Â¿Puedes decir por quÃ© `malloc` en C++ para objetos es Undefined Behavior?
- [ ] Â¿Puedes dar un ejemplo de un sistema donde C serÃ­a la elecciÃ³n correcta sobre Rust?
- [ ] Â¿Sabes quÃ© es RAII y en quÃ© lenguaje(s) aplica?
- [ ] Â¿Puedes explicar por quÃ© un data race es un error de compilaciÃ³n en Rust pero no en C++?

---

*Siguiente tema sugerido: `02_variables_y_tipos.md` â€” el sistema de tipos de C++ y cÃ³mo difiere de C.*


