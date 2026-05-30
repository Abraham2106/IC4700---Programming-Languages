# Smart Pointers — Gestión automática de propiedad con unique_ptr, shared_ptr y weak_ptr

Los smart pointers de C++ automatizan la gestión de la propiedad de memoria en el heap aplicando RAII. `std::unique_ptr` garantiza una única referencia exclusiva, mientras que `std::shared_ptr` y `std::weak_ptr` manejan propiedad compartida y previenen ciclos de referencia.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
Un puntero inteligente es un objeto del stack que emula la interfaz de un puntero nativo (vía operadores `->` y `*`), pero cuyo destructor destruye el objeto apuntado en el heap de forma determinista.

### 1.2 ¿Por qué importa?
- **Seguridad ante Excepciones:** Si una función lanza una excepción a la mitad de su ejecución, las variables del stack se limpian automáticamente (desenrollado de pila o *stack unwinding*), lo que destruye el smart pointer y libera la memoria. Un puntero nativo huérfano Únicamente causaría un leak permanente.
- **Evitar fugas complejas:** Permiten automatizar la vida de estructuras de datos compartidas mediante esquemas basados en contadores de referencias dinámicos.

---

## 2. Hook Example

```cpp
#include <iostream>
#include <memory>

class RecursoCritico {
public:
    RecursoCritico() { std::cout << "[ALLOC] Recurso adquirido en heap.\n"; }
    ~RecursoCritico() { std::cout << "[FREE] Recurso destruido automaticamente.\n"; }
    
    void procesar() const {
        std::cout << "Procesando datos criticos...\n";
    }
};

void funcionConRiesgo() {
    // std::unique_ptr posee exclusivamente la memoria. Al salir de scope, se destruye el objeto.
    std::unique_ptr<RecursoCritico> ptr = std::make_unique<RecursoCritico>();
    
    ptr->procesar(); // Acceso identico a un puntero nativo

    // Si lanzamos un error aqui, ptr aun asi se destruira limpiando la memoria
    throw std::runtime_error("Fallo inesperado.");
}

int main() {
    try {
        funcionConRiesgo();
    } catch (const std::exception& e) {
        std::cout << "Excepcion atrapada: " << e.what() << "\n";
    }
    return 0;
}
```

---

## 3. Descomposición Under the Hood

### 3.1 ¿Cómo funciona físicamente un `std::shared_ptr`?
Un `shared_ptr` no contiene un único puntero interno. Físicamente, es un objeto que contiene **dos** punteros nativos:
1. Un puntero al objeto administrado (`T*`).
2. Un puntero al **Bloque de Control** (Control Block) ubicado en otra sección del Heap.

#### Anatomía del Bloque de Control:
El bloque de control es una estructura dinámica compartida por todos los `shared_ptr` y `weak_ptr` que apuntan al mismo objeto:

```
┌────────────────────────────────────────────────────────┐
│ shared_ptr<T>                                          │
│   T* ptr ────────────────────────┐                     │
│   ControlBlock* ctrlBlock ───────┼─────────┐           │
└──────────────────────────────────│─────────│───────────┘
                                   │         │
                                   ▼         ▼
                            ┌──────────┐ ┌──────────────────────────────────────────────┐
                            │ Objeto T │ │ Bloque de Control (Heap)                     │
                            │  (Heap)  │ │   Strong Ref Count (Contador shared) : 2     │
                            └──────────┘ │   Weak Ref Count (Contador weak)     : 1     │
                                         │   Custom Deleter / Allocator                 │
                                         └──────────────────────────────────────────────┘
```

- **Strong Ref Count:** Contador de `shared_ptr` activos. Cuando llega a 0, se destruye el objeto `T`.
- **Weak Ref Count:** Contador de `weak_ptr` activos. El bloque de control solo se destruye en el heap cuando el Strong Count y el Weak Count llegan ambos a 0.
- **Acceso Atómico:** Las modificaciones a los contadores de referencias se realizan mediante operaciones atómicas de CPU (como `lock xadd` en x86) para garantizar la consistencia en aplicaciones multihilo sin necesidad de usar mutexes explícitos de software.

---

## 4. Teoría: Smart Pointers de la STL

C++11 introdujo tres smart pointers estándar dentro del encabezado `<memory>`:

### 4.1 1. `std::unique_ptr`
- **Semántica:** Propiedad exclusiva. No se puede copiar, solo mover (`std::move`).
- **Costo:** Cero overhead de memoria (8 bytes) y llamadas inline directas.

### 4.2 2. `std::shared_ptr`
- **Semántica:** Propiedad compartida.
- **Costo:** Ocupa 16 bytes (dos punteros) en el stack, además del overhead del bloque de control en el heap y operaciones atómicas en los contadores.

### 4.3 3. `std::weak_ptr`
- **Semántica:** Observador sin propiedad. No incrementa el contador shared, previniendo ciclos de referencia circulares.

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Diagnóstico de accesos nulos
En debug, un smart pointer puede ser configurado para interceptar desreferenciaciones accidentales:
```cpp
template <class T>
class PtrSeguro {
    T* rawPtr;
public:
    T* operator->() {
        if (rawPtr == nullptr) {
            std::cerr << "Intento de dereferencia NULL!\n";
            rawPtr = new T(); // Auto-instanciacion perezosa para evitar caida
        }
        return rawPtr;
    }
};
```

### 5.2 Nivel Aplicado: Screamers de Auditoría
El libro *Secrets of C++ Masters* presenta la técnica de los **Screamers**. Si el puntero inteligente apunta a `nullptr` y se intenta desreferenciar, el operador no lanza una excepción estándar ni se cae. En su lugar, devuelve la dirección de un objeto estático global especial ("Screamer").
- Este objeto es de una clase derivada cuyos métodos virtuales han sido modificados para escribir diagnósticos de alerta críticos (`std::cerr << "Fallo!"`) antes de delegar la llamada a una rutina segura, permitiendo depurar sistemas complejos en producción sin detener el proceso.

### 5.3 Nivel Complejo: Optimización mediante `std::make_shared`
Cuando usas `std::shared_ptr<T>(new T())`, el compilador realiza dos asignaciones independientes en el heap: una para el objeto `T` y otra para el bloque de control.
- Usar `std::make_shared<T>()` realiza **una única asignación contigua** de memoria en el heap lo suficientemente grande para contener ambos elementos.
- Esto reduce el overhead del asignador del sistema operativo y mejora la localidad de caché de la CPU.

---

## 6. Diseño de Sistemas
En el diseño de sistemas concurrentes (como servidores web), los smart pointers evitan el problema de *Dangling Reference* donde un hilo libera la memoria de una sesión de conexión mientras otro hilo intenta leer el socket. El uso de `shared_ptr` garantiza que la memoria del socket se mantenga con vida hasta que el último hilo de servicio finalice su ejecución.

---

## Exercises

### Exercise 1 — Simulación de ciclos de referencia
Crea una estructura `Persona` que contenga un `std::shared_ptr<Persona> amigo`. En `main()`, crea dos personas en el heap que se apunten mutuamente. Comprueba si sus destructores se llaman al salir de scope. Cambia la relación a `std::weak_ptr` y analiza los resultados.

---

## 7. Errores Comunes y Anti-Patrones
- **Uso de `shared_ptr` por defecto:** Produce overhead innecesario en sistemas monohilo. Utiliza `unique_ptr` a menos que realmente compartas la propiedad del recurso.
- **Crear shared_ptr desde punteros temporales crudos:** Escribir `shared_ptr<T>(ptr)` en múltiples lugares independientes genera múltiples bloques de control para la misma dirección física, rompiendo el contador de referencias.

---

## 8. Conclusión

---

---

*Next: `31 — Parametrized smart pointers y handles y facets.md` — Abstracciones complejas de recursos.*
