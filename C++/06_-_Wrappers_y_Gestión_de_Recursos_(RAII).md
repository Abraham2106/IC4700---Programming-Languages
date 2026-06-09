# Wrappers y Gestión de Recursos (RAII) — Gestión automática y determinista del ciclo de vida de los recursos

El principio RAII vincula la adquisición y liberación de recursos del sistema directamente al tiempo de vida de los objetos en el stack. Al encapsular recursos en constructores y liberarlos en destructores, se evitan fugas de memoria y bloqueos incluso ante la ocurrencia de excepciones.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
Un **Wrapper** es una clase que envuelve un recurso de bajo nivel (como memoria dinámica, un socket de red, o un mutex) para proveer una interfaz limpia y automatizada. **RAII** es la técnica de diseño que dicta que el constructor adquiere el recurso y el destructor lo libera de forma garantizada cuando el objeto sale de ámbito (scope).

### 1.2 ¿Por qué importa?
En lenguajes como C, olvidar un `free` o un `close` causa fugas de memoria (memory leaks) y cuelgues del sistema. RAII elimina esta clase de errores en C++ haciendo la liberación determinista y automática sin requerir un Garbage Collector.

---

## 2. Hook Example

```cpp
#include <iostream>
#include <fstream>

// Wrapper RAII personalizado para un archivo
class LectorArchivo {
    std::ifstream archivo;
public:
    LectorArchivo(const std::string& ruta) {
        archivo.open(ruta); // Adquisición del recurso
    }
    ~LectorArchivo() {
        if (archivo.is_open()) {
            archivo.close(); // Liberación automática del recurso
            std::cout << "Archivo cerrado automaticamente.\n";
        }
    }
};

void procesar() {
    LectorArchivo lector("datos.txt");
    // Si ocurre un error o se lanza una excepcion aqui,
    // el destructor de `lector` se ejecuta de todos modos al salir de la funcion.
}
```

---

## 3. Descomposición Under the Hood

### 3.1 ¿Cómo garantiza C++ la ejecución del destructor?
- El compilador inyecta llamadas implícitas a los destructores de todas las variables locales (alojadas en el Stack) justo antes del retorno de una función o al salir de un bloque de llaves `{}`.
- **Stack Unwinding (Desenrollado de pila):** Si una función lanza una excepción, el runtime de C++ recorre el Stack hacia atrás buscando un manejador de excepciones (`catch`). Durante este proceso, destruye en orden inverso de creación todos los objetos locales válidos activos en las funciones que se van abandonando. Esto previene fugas de recursos en presencia de excepciones.

---

## 4. Teoría: RAII y Smart Pointers

### 4.1 1. El ciclo de vida de RAII
- **Construcción:** El objeto se aloca en el Stack. Su constructor reserva memoria o adquiere handles en el Heap o el SO.
- **Uso:** Acceso seguro al recurso a través de métodos de clase.
- **Destrucción:** Al salir del scope, el Stack libera la variable llamando a su destructor, el cual libera internamente el recurso adquirido.

### 4.2 2. Smart Pointers (C++11)
El estándar de C++ provee wrappers RAII genéricos para manejar memoria dinâmica:
- `std::unique_ptr<T>`: Representa propiedad exclusiva del recurso. No se puede copiar, solo mover (`std::move`).
- `std::shared_ptr<T>`: Representa propiedad compartida. Usa conteo de referencias (reference counting). El recurso se libera cuando el último `shared_ptr` es destruido.
- `std::weak_ptr<T>`: Referencia no propietaria a un objeto manejado por un `shared_ptr` para evitar referencias circulares.

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: `unique_ptr` básico
```cpp
#include <memory>

void usar_memoria() {
    // Aloca un entero en el heap envuelto en un unique_ptr
    std::unique_ptr<int> ptr = std::make_unique<int>(42);
    // No hay necesidad de llamar a delete. unique_ptr lo hace al finalizar el bloque.
}
```

### 5.2 Nivel Aplicado: Control de Concurrencia con `std::lock_guard`
```cpp
#include <mutex>

std::mutex mtx;

void seccion_critica() {
    // El constructor bloquea el mutex. El destructor lo desbloquea.
    std::lock_guard<std::mutex> lock(mtx);
    // Operaciones seguras con hilos...
}
```

### 5.3 Nivel Complejo: Custom Deleters en Smart Pointers
Podemos usar `std::unique_ptr` con recursos legados de C usando destructores personalizados.
```cpp
#include <memory>
#include <cstdio>

struct FileDeleter {
    void operator()(FILE* f) const {
        if (f) {
            fclose(f);
            std::cout << "FILE cerrado exitosamente.\n";
        }
    }
};

void leer_legado() {
    // std::unique_ptr maneja un handle de C clásico (FILE*) de forma segura
    std::unique_ptr<FILE, FileDeleter> uptr(fopen("config.ini", "r"));
}
```

---

## 6. Diseño de Sistemas
En servidores de alto rendimiento, RAII es crítico para evitar fugas de descriptores de archivos de red (sockets). Cada conexión de red se modela como un objeto cuya destrucción cierra el canal de red de forma segura.

---

## Exercises

### Exercise 1 — Implementar un Wrapper RAII de Mutex
Implementa tu propio wrapper simplificado para simular el comportamiento de `std::lock_guard`.

```cpp
#include <iostream>

// Simulador de Mutex físico
class MutexSimulado {
public:
    void lock() { std::cout << "Mutex bloqueado.\n"; }
    void unlock() { std::cout << "Mutex desbloqueado.\n"; }
};

// TODO: Crear la clase "BloqueadorSeguro" que reciba una referencia a MutexSimulado,
// lo bloquee en el constructor y lo desbloquee automáticamente en el destructor.
class BloqueadorSeguro {
    // Implementar
};

int main() {
    MutexSimulado mtx;
    {
        // BloqueadorSeguro lock(mtx);
        std::cout << "Ejecutando operaciones...\n";
    }
    // Debería imprimirse "Mutex desbloqueado" justo después de salir del bloque.
    return 0;
}
```

---

## 7. Errores Comunes y Anti-Patrones
- **Alocar Wrappers RAII con `new`:** Escribir `auto lock = new std::lock_guard(mtx);` rompe por completo el propósito de RAII, ya que el destructor de Stack no se invocará automáticamente y el recurso quedará bloqueado indefinidamente.
- **Referencias circulares con `shared_ptr`:** Si el objeto A tiene un `shared_ptr` a B, y B tiene uno a A, el contador de referencias nunca llegará a cero y se producirá una fuga de memoria grave. Se debe romper el ciclo usando `std::weak_ptr`.

---

## 8. Conclusión

---

---

*Next: `07 — Ámbito y Ciclo de Vida (Scope).md` — Variables en stack, heap y estáticas.*
