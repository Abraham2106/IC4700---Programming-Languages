# 23 — Destructores

> **Resumen Ejecutivo:** Los destructores en C++ se encargan de liberar los recursos adquiridos por un objeto al final de su ciclo de vida. Aunque la invocación implícita (automática) es el estándar y la práctica recomendada bajo RAII, C++ permite la invocación explícita de destructores para escenarios avanzados de gestión de memoria como *placement new*. Esta nota analiza las sutilezas mecánicas del compilador durante la destrucción y el peligro de desajustes de tamaño en asignadores de memoria personalizados.
>
> **Prerrequisitos:** Haber leído [16 — Orden de Construcción y Destrucción (Depth)](<16 — Orden de Construcción y Destrucción (Depth).md>) y [20 — Operadores new y delete](<20 — Operadores new y delete.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Invocación Implícita vs. Explícita](#teoría-invocación-implícita-vs-explícita)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
Un destructor es una función miembro especial que limpia el objeto antes de que su memoria física sea devuelta al sistema. Se nombra con una tilde (`~`) seguida del nombre de la clase, no toma parámetros y no tiene tipo de retorno. No puede ser sobrecargado ni declarado como `const`, `volatile` o `static`.

### ¿Por qué importa?
- **Evitar fugas de recursos:** Libera memoria dinámica, cierra descriptores de archivos, sockets o mutexes.
- **Ciclo de vida determinista:** A diferencia de lenguajes con Garbage Collector, en C++ la destrucción ocurre en un punto exacto y predecible del tiempo de ejecución.
- **Acoplamiento con Custom Allocators:** Si no se gestiona de forma correcta la herencia con destructores virtuales, las rutinas de desasignación a nivel de sistema recibirán tamaños de liberación erróneos.

---

## Conceptos Previos
- Bloques de ámbito (scope) y variables locales (stack-allocated).
- Concepto de *placement new* (construcción de un objeto sobre una dirección de memoria pre-asignada).
- Alineación de tipos (`alignof`) y cómo afecta la asignación de buffers.

---

## Hook Example

```cpp
#include <iostream>
#include <new> // Para placement new
#include <string>

class ElementoAuditado {
    std::string identificador;
public:
    ElementoAuditado(std::string id) : identificador(id) {
        std::cout << "[CONSTRUCTOR] Elemento '" << identificador << "' construido.\n";
    }

    ~ElementoAuditado() {
        std::cout << "[DESTRUCTOR] Elemento '" << identificador << "' destruido.\n";
    }
};

int main() {
    // 1. Invocación Implícita (Automática al salir de scope)
    {
        ElementoAuditado eStack("StackObject"); 
    } // ~ElementoAuditado() se llama aquí automáticamente por el compilador.

    std::cout << "--- Fin de la seccion de Stack ---\n";

    // 2. Invocación Explícita (Requerida en buffers pre-asignados / Placement New)
    // Reservamos memoria sin inicializar en el stack usando alineacion correcta
    alignas(ElementoAuditado) char buffer[sizeof(ElementoAuditado)];
    
    // Construimos el objeto en la direccion de memoria pre-existente
    std::cout << "Llamando a placement new...\n";
    ElementoAuditado* ePlacement = new (buffer) ElementoAuditado("PlacementObject");

    // ¡CRÍTICO! No podemos usar 'delete ePlacement' porque 'buffer' está en la pila y delete intentará 
    // liberar la direccion en el heap usando free(). Debemos destruir el objeto manualmente:
    std::cout << "Destruyendo explicitamente...\n";
    ePlacement->~ElementoAuditado(); 

    return 0;
}
```

---

## Descomposición Under the Hood

### ¿Qué sucede durante la destrucción?
Cuando un objeto es destruido (ya sea por fin de scope, `delete` o llamada explícita):
1. **Ejecución del cuerpo:** Se ejecuta el código que el desarrollador escribió dentro del cuerpo de `~Clase()`.
2. **Destrucción de miembros:** Los objetos miembro no estáticos de la clase se destruyen en el orden **exactamente inverso** al de su declaración en la clase (no en el orden de la lista de inicializadores).
3. **Destrucción de bases:** Se invocan los destructores de las clases base en orden inverso a su declaración en la lista de herencia.
4. **Bases virtuales:** Si hay herencia virtual, los destructores de las bases virtuales se ejecutan al final del proceso.
5. **Liberación física (si aplica):** Si la destrucción fue disparada por `delete ptr`, el compilador inserta una llamada a la función desasignadora `operator delete(ptr)` después de completar el paso 4.

### Diagrama de Flujo del compilador ante `delete ptr;`
```
   [delete ptr;]
         │
         ▼
[¿Es ptr nullptr?] ──(Sí)──> [No hacer nada]
         │ (No)
         ▼
[Llamar al Destructor] ────> [Ejecutar cuerpo del ~T()]
         │                               │
         ▼                               ▼
[Llamar operator delete(ptr)]  [Destruir miembros (orden inverso)]
                                         │
                                         ▼
                               [Destruir clases base]
```

---

## Teoría: Invocación Implícita vs. Explícita

### Invocación Implícita
Ocurre automáticamente en los siguientes casos:
- **Ámbito Local (Stack):** Al alcanzar la llave de cierre `}` del bloque de código.
- **Objetos Temporales/Anónimos:** Al final de la expresión en la cual fueron generados (ej. `evaluar(Point(1,2));`).
- **Miembros de Clases:** Cuando el objeto contenedor (host) finaliza su ciclo de vida.
- **Heap (vía delete):** El operador `delete` actúa en dos pasos: primero llama al destructor del tipo dinámico y luego libera el bloque de memoria subyacente.

### Invocación Explícita (`ptr->~Clase()`)
Es una herramienta avanzada que se usa **exclusivamente** cuando la asignación de memoria (allocation) y la construcción (initialization) han sido desacopladas (típicamente mediante *placement new*).
- **Regla de oro:** Si usas placement new (`new (ptr) T()`), **debes** llamar explícitamente al destructor (`ptr->~T()`) y **nunca** usar `delete ptr` en esa dirección a menos que quieras liberar memoria que no fue obtenida del heap de forma estándar.

---

## Progresión de Complejidad

### Nivel Simple: Destructores por Defecto y Triviales
Si no defines un destructor, el compilador genera uno implícito por defecto que llama a los destructores de todos los miembros no estáticos y clases base. Si todos los miembros y bases de una clase tienen destructores triviales (ej. tipos primitivos `int`, `double`, etc.), el destructor de la clase se clasifica como **trivial** y el compilador puede omitir por completo la inserción de código de destrucción en ensamblador.

### Nivel Aplicado: Destructores Virtuales y el Desajuste de Tamaños
El libro *Secrets of C++ Masters* resalta un problema muy destructivo y poco conocido: si destruyes un objeto derivado a través de un puntero de clase base sin destructor virtual, el compilador llamará al destructor base (causando fugas de los miembros de la derivada) pero además:
- Si implementas un gestor de memoria personalizado (`operator delete` sobrecargado con segundo parámetro de tamaño: `void operator delete(void* p, size_t size)`), el compilador le pasará a tu gestor el tamaño de la clase **Base** en lugar de la clase **Derivada**.
- Tu gestor intentará registrar que se liberaron, por ejemplo, 24 bytes en lugar de los 256 reales, desestabilizando completamente las estadísticas de pools de memoria y causando corrupción en asignaciones posteriores.

### Nivel Complejo: Destructores y Excepciones (Regla Invariable)
Un destructor **nunca** debe propagar una excepción fuera de sí mismo.
- Si un destructor lanza una excepción mientras el stack ya se está desenrollando debido a *otra* excepción previa, el tiempo de ejecución de C++ llamará inmediatamente a `std::terminate()`, abortando la aplicación sin remedio.
- En C++11 y posteriores, todos los destructores se consideran de forma implícita como `noexcept(true)` a menos que se especifique lo contrario. Cualquier excepción lanzada dentro de ellos que intente salir del cuerpo disparará la terminación inmediata del proceso.

---

## Diseño de Sistemas
En el diseño de contenedores de datos de alto rendimiento (como `std::vector`), se desacopla la reserva de memoria de la construcción. Se usa un buffer continuo de bytes crudos (`std::allocator` o `malloc`), se instancian los objetos perezosamente con *placement new*, y cuando el usuario llama a `.clear()`, se invocan sus destructores de forma explícita uno por uno sin liberar el bloque de memoria subyacente para evitar llamadas costosas al sistema operativo.

---

## Ejercicios

### Ejercicio 1 — Identificar el error de doble destrucción
¿Por qué el siguiente código produce un comportamiento indefinido en ejecución? Corrige el error para que use placement new de forma segura en un buffer alineado.

```cpp
#include <iostream>
#include <string>

class Recurso {
    std::string nombre;
public:
    Recurso(std::string n) : nombre(n) {}
    ~Recurso() {
        std::cout << "Destruyendo " << nombre << "\n";
    }
};

int main() {
    Recurso r("Local");
    r.~Recurso(); // Llamada explicita manual
    
    // ¿Que pasa cuando 'r' sale de scope aqui?
    return 0;
}
```

---

## Errores Comunes y Anti-Patrones
- **Llamar a `delete` sobre un puntero inicializado con placement new:** Causa corrupción de memoria catastrófica.
- **Olvidar los destructores virtuales en clases base polimórficas:** Produce memory leaks e informa tamaños erróneos a los asignadores personalizados de memoria.
- **Dejar que una excepción escape de un destructor:** Causa abortos inexplicables de la aplicación en producción.

---

## Conclusión y Checklist Mental
- [ ] ¿Entiendes por qué no se debe llamar al destructor de forma explícita en objetos normales del stack o del heap asignados con `new` ordinario?
- [ ] ¿Cómo garantiza C++ el orden de destrucción inverso a la construcción?
- [ ] ¿Qué pasa con el tamaño devuelto al asignador de memoria si se destruye un objeto a través de un puntero base sin un destructor virtual?
- [ ] ¿Por qué los destructores son implícitamente `noexcept`?

---

*Siguiente tema sugerido: [24 — Asignación y Semántica de Copia](<24 — Asignacion  ( tipo de semantica ( reglas no obcias, copy by value, copy by reference, comapradores, copias hibirdas ) x = y not the same ) ) , DUmp.md>)*
