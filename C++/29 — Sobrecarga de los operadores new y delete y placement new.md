# 29 — Sobrecarga de `new`, `delete` y `placement new`

> **Resumen Ejecutivo:** C++ permite la sobrecarga local (por clase) y global de los operadores `new` y `delete`. Esta característica permite reemplazar el asignador de memoria por defecto por estrategias altamente optimizadas, como *Free Lists* (listas libres) y *Placement New*, reduciendo el overhead de asignación y fragmentación en sistemas de alto rendimiento. Esta nota detalla la implementación de listas libres robustas y los requerimientos críticos de alineación física de memoria.
>
> **Prerrequisitos:** Haber leído [19 — Gestión de Memoria Stack vs Heap](<19 — Gestión de Memoria Stack vs Heap.md>), [20 — Operadores new y delete](<20 — Operadores new y delete.md>) y [23 — Destructores (Invocación Directa, Implícita y Explicita)](<23 — Destructores (Invocacion directa de destructores Explicita e Implicita, definicion de operadores).md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Sobrecarga de new y delete](#teoría-sobrecarga-de-new-y-delete)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
La sobrecarga de `new` y `delete` consiste en redefinir las funciones de asignación y liberación de memoria física (`void*`). C++ divide la instanciación en dos pasos: obtener el bloque físico de bytes e invocar al constructor. La sobrecarga permite controlar el primer paso.

### ¿Por qué importa?
- **Reducción de Latencia:** El asignador por defecto del sistema operativo realiza llamadas al sistema complejas buscando bloques libres. Un asignador especializado basado en *Free Lists* puede resolver solicitudes en tiempo constante $O(1)$ sin llamar al núcleo del sistema operativo.
- **Control de Fragmentación:** Permite confinar la asignación de una clase a bloques contiguos de memoria pre-reservados.

---

## Conceptos Previos
- Alineación de memoria (`alignof`, `alignas`, `std::max_align_t`).
- Destructores virtuales y su impacto en la determinación del tamaño en tiempo de ejecución.
- Punteros genéricos (`void*`) y su conversión explícita.

---

## Hook Example

```cpp
#include <iostream>
#include <new>
#include <cstddef>

class ElementoEspecial {
private:
    // Estructura superpuesta en la memoria inactiva del objeto
    struct NodoLibre {
        NodoLibre* siguiente;
    };
    static NodoLibre* listaLibre;

    int valor;
    double datosExtra[4]; // Garantiza tamaño suficiente para contener un puntero
public:
    explicit ElementoEspecial(int v) : valor(v) {}

    // Destructor virtual para asegurar calculos de tamano correctos en delete
    virtual ~ElementoEspecial() {}

    // 1. Sobrecarga de operator new para la clase
    void* operator new(size_t bytes) {
        // Si el tamaño no coincide (debido a herencia) o la lista está vacía, delegamos al global
        if (bytes != sizeof(ElementoEspecial) || listaLibre == nullptr) {
            std::cout << "[ALLOC] Asignacion global de " << bytes << " bytes.\n";
            return ::operator new(bytes);
        }
        std::cout << "[ALLOC] Reutilizando espacio de la Free List.\n";
        NodoLibre* temp = listaLibre;
        listaLibre = listaLibre->siguiente;
        return temp;
    }

    // 2. Sobrecarga de operator delete para la clase
    void operator delete(void* direccion, size_t bytes) {
        if (bytes != sizeof(ElementoEspecial)) {
            std::cout << "[FREE] Liberacion global de memoria.\n";
            ::operator delete(direccion);
            return;
        }
        std::cout << "[FREE] Guardando memoria en la Free List.\n";
        NodoLibre* nodo = static_cast<NodoLibre*>(direccion);
        nodo->siguiente = listaLibre;
        listaLibre = nodo;
    }
};

// Inicialización de la variable estática
ElementoEspecial::NodoLibre* ElementoEspecial::listaLibre = nullptr;

int main() {
    std::cout << "--- Asignacion 1 ---\n";
    ElementoEspecial* e1 = new ElementoEspecial(10);

    std::cout << "--- Destruccion 1 ---\n";
    delete e1; // La memoria va a la lista libre, no al OS

    std::cout << "--- Asignacion 2 ---\n";
    ElementoEspecial* e2 = new ElementoEspecial(20); // Reutiliza memoria
    delete e2;

    return 0;
}
```

---

## Descomposición Under the Hood

### Flujo Físico de Memoria en una Free List
Cuando la memoria de un objeto `ElementoEspecial` está inactiva (después de llamarse a `delete`), reutilizamos esos mismos bytes físicos para almacenar la estructura `NodoLibre`:

```
OBJETO ACTIVO EN MEMORIA:
┌────────────────────────────────────────────────────────┐
│  vptr (8 bytes)  │  valor (4 bytes)  │ ...datosExtra   │
└────────────────────────────────────────────────────────┘

OBJETO INACTIVO (EN LA FREE LIST):
┌────────────────────────────────────────────────────────┐
│  NodoLibre* siguiente (8 bytes)  │ ...memoria residual │
└────────────────────────────────────────────────────────┘
```
- Esto significa que la lista libre **no tiene overhead de memoria** en el heap; la propia memoria del objeto inactivo sirve de estructura de enlace para la lista.

---

## Teoría: Sobrecarga de new y delete

### Requerimientos de Alineación (Alignment)
El estándar de C++ exige que cualquier función de asignación de memoria (`operator new`) devuelva un puntero alineado al tipo de datos correspondiente.
- Si sobrecargas `operator new` de forma global o mediante pools de memoria personalizados, debes garantizar que las direcciones devueltas cumplan con `alignof(T)`.
- Si devuelves un puntero desalineado (por ejemplo, una dirección impar para un tipo que requiere alineación de 8 bytes como `double`), la CPU tendrá que realizar múltiples accesos a memoria por lectura, reduciendo drásticamente el rendimiento, o provocando fallos físicos de bus en arquitecturas ARM o RISC.

---

## Progresión de Complejidad

### Nivel Simple: Free Lists Básicas
Se basan en reutilizar el espacio de los objetos destruidos. Para garantizar que funcione, el tamaño de la clase (`sizeof(T)`) debe ser al menos igual o mayor al tamaño de un puntero (`sizeof(void*)`). Si la clase es vacía, el compilador suele asignarle un tamaño de 1 o 2 bytes. Forzar la presencia de una función virtual añade la dirección del puntero a la *vtable* (8 bytes), solucionando la restricción de tamaño mínimo.

### Nivel Aplicado: Placement New con Pools de Memoria
Podemos parametrizar el operador `new` para que el cliente decida de qué pool de memoria asignar recursos.
- Esto permite aislar la creación de objetos en la pila física o en segmentos contiguos del heap:
```cpp
struct MemoryArena {
    char buffer[1024];
    size_t offset = 0;
    void* alloc(size_t bytes) {
        void* p = &buffer[offset];
        offset += bytes;
        return p;
    }
};

void* operator new(size_t bytes, MemoryArena* arena) {
    return arena->alloc(bytes);
}
```

### Nivel Complejo: Sobrecarga de arrays (`operator new[]` y `operator delete[]`)
Cuando asignas un arreglo con `new T[N]`, el compilador a menudo almacena el número de elementos `N` en una palabra oculta (*cookie*) al inicio del bloque de memoria física asignado, devolviendo al usuario la dirección desplazada.
- Si sobrecargas `operator new[]`, debes prever que el tamaño total solicitado por el compilador puede ser mayor que `N * sizeof(T)` debido a esta cookie.
- Si tu asignador personalizado no devuelve memoria alineada previendo este desplazamiento, la aplicación fallará.

---

## Diseño de Sistemas
En el diseño de sistemas críticos o en tiempo real (misión crítica), está prohibido realizar asignaciones dinámicas libres. Se usan pre-asignadores gigantes en el arranque del sistema y placement new sobre estructuras fijas para eliminar por completo la fragmentación y garantizar predictibilidad temporal absoluta.

---

## Ejercicios

### Ejercicio 1 — Analizar la cookie en sobrecarga de arrays
Crea una clase simple e implementa sobrecargas locales para `operator new[]` y `operator delete[]` imprimiendo el tamaño de bytes solicitado. En el programa principal instancía un arreglo `new Clase[5]`. Compara si el tamaño solicitado es exactamente `5 * sizeof(Clase)` o si el compilador solicitó bytes adicionales para la cookie.

---

## Errores Comunes y Anti-Patrones
- **No verificar diferencias de tamaño en clases heredadas:** Causa desbordamientos de búfer en memoria si una clase derivada más grande intenta asignarse usando el espacio predefinido para la clase base.
- **Ignorar las restricciones de alineación:** Produce reducciones de rendimiento críticas o fallas físicas en procesadores móviles/bebidos.

---

## Conclusión y Checklist Mental
- [ ] ¿Cómo divide C++ la creación de un objeto en dos fases diferenciadas?
- [ ] ¿Por qué una clase con asignación customizada debe tener un destructor virtual si se planea heredar de ella?
- [ ] ¿Qué es la "cookie" en la asignación de arreglos y cómo afecta el tamaño solicitado?
- [ ] ¿Por qué es crítico respetar la alineación de memoria (`alignof`) al devolver punteros en `operator new`?

---

*Siguiente tema sugerido: [30 — Smart Pointers](<30 — Smart Pointers.md>)*
