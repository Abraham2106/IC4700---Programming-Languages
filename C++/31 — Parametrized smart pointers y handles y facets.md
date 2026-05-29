# 31 — Smart Pointers, Handles y Facets

> **Resumen Ejecutivo:** Los punteros inteligentes parametrizados y los manejadores (*Handles*) son abstracciones avanzadas de bajo nivel en C++ que permiten desacoplar al cliente de la gestión directa del ciclo de vida y la ubicación de los objetos en memoria. Esta nota analiza en profundidad el uso de Handles para implementar compactación de memoria (como el algoritmo de Baker), el desacoplamiento mediante Facetas y la reducción del acoplamiento de interfaces a una escala $O(N)$ usando Gemstones.
>
> **Prerrequisitos:** Haber leído [30 — Smart Pointers](<30 — Smart Pointers.md>) y [32 — Templates en C++](<32 — Templates en C++.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Handles, Facets y Gemstones](#teoría-handles-facets-y-gemstones)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
- **Manejador (Handle):** Es un puntero indirecto (puntero a puntero). En lugar de apuntar directamente al objeto destino, el Handle apunta a un *Master Pointer*. Esto permite mover el objeto destino en memoria física (para compactación o reubicación) actualizando una única dirección de memoria intermedia, sin invalidar los punteros de los clientes.
- **Faceta (Facet):** Es un smart pointer especializado que expone únicamente un subconjunto de la interfaz total de un objeto pesado (por ejemplo, separar las funciones de dibujo de las funciones de eventos en una clase `View`).
- **Gemstone (Piedra preciosa):** Un objeto coordinador que centraliza la administración y conversión entre múltiples facetas para un mismo objeto subyacente.

### ¿Por qué importa?
- **Compactación de Memoria:** Permite reubicar dinámicamente objetos para consolidar el espacio libre del Heap (desfragmentación) sin romper las referencias de los clientes.
- **Factorización de Interfaces:** En clases complejas con docenas de métodos (como controles de interfaz gráfica), las facetas evitan compilar interfaces gigantescas, reduciendo dependencias en el tiempo de compilación.

---

## Conceptos Previos
- Operadores de conversión implícitos de clases.
- La directiva `friend` para delegar construcciones privadas.
- Patrón de herencia múltiple de interfaces.

---

## Hook Example

```cpp
#include <iostream>

// Clase oculta del cliente
class VistaInterna {
public:
    void dibujar() { std::cout << "Dibujando vista en pantalla.\n"; }
    void procesarEvento() { std::cout << "Procesando evento de entrada.\n"; }
};

// Facetas (Interface Pointers)
class FacetaDibujo {
    VistaInterna* vista;
public:
    explicit FacetaDibujo(VistaInterna* v) : vista(v) {}
    void dibujar() { vista->dibujar(); }
};

class FacetaEventos {
    VistaInterna* vista;
public:
    explicit FacetaEventos(VistaInterna* v) : vista(v) {}
    void procesarEvento() { vista->procesarEvento(); }
};

// Coordinador Gemstone que administra las facetas
class VistaGemstone {
    VistaInterna* vista;
public:
    explicit VistaGemstone(VistaInterna* v) : vista(v) {}
    ~VistaGemstone() { delete vista; }

    operator FacetaDibujo() { return FacetaDibujo(vista); }
    operator FacetaEventos() { return FacetaEventos(vista); }
};

int main() {
    VistaGemstone g(new VistaInterna());

    // El cliente solo solicita las facetas que necesita (Interface Slicing)
    FacetaDibujo dibujador = g; // Conversion implicita
    dibujador.dibujar();

    FacetaEventos controlador = g;
    controlador.procesarEvento();

    return 0;
}
```

---

## Descomposición Under the Hood

### Compactación física usando Handles y el Algoritmo de Baker
En sistemas embebidos de tiempo real que no pueden tolerar la fragmentación del Heap, se implementa la recolección de basura por copia (como el Algoritmo de Baker).
- El Heap se divide en dos espacios semiesféricos: *From-Space* y *To-Space*.
- Durante la compactación, los objetos activos se copian del *From-Space* al *To-Space* de forma contigua, eliminando los huecos.
- Si los clientes tuvieran punteros directos (`T*`), reubicar el objeto requeriría buscar y actualizar cada puntero en el código, lo cual es inviable.
- Con Handles, el compilador accede de forma indirecta:

```
CLIENTE (Handle)                     TABLA DE MASTER POINTERS                  HEAP FISICO (To-Space)
┌─────────────────┐                  ┌───────────────────────┐                 ┌───────────────────────┐
│  H<T>           │                  │ MP<T>                 │                 │ Objeto T              │
│    MP<T>* ptr ──┼─────────────────>│   T* rawPtr ──────────┼────────────────>│   ...estado actual    │
└─────────────────┘                  └───────────────────────┘                 └───────────────────────┘
                                                                (Solo se actualiza este puntero en la compactación)
```

Cualquier cambio de dirección física solo requiere modificar el `rawPtr` del `MP<T>` centralizado. Los Handles de los clientes se mantienen intactos.

---

## Teoría: Handles, Facets y Gemstones

### 1. Desacoplamiento mediante Facetas (Facets)
Cuando una clase (como un control de ventana `View`) tiene más de 80 funciones miembro, compilar sus cabeceras expone demasiados detalles lógicos internos.
- Se crean facetas (`ViewDrawing`, `ViewEvents`, `ViewGeometry`) que replican solo un subconjunto de funciones y delegan internamente al objeto real.
- El código cliente que solo dibuja solo incluye `ViewDrawing.h`, evitando recompilar si cambian los eventos de entrada de la ventana.

### 2. Escalabilidad con Gemstones
Si tienes $N$ facetas y deseas permitir que el usuario obtenga una faceta a partir de otra de forma directa (por ejemplo, obtener la interfaz de eventos desde la de dibujo), escribir conversiones directas entre todas requiere programar $N(N-1)$ operadores de conversión (complejidad $O(N^2)$).
- **Solución Gemstone:** Cada faceta implementa una única conversión a un tipo común `Gemstone`. El `Gemstone` a su vez tiene conversiones a todas las facetas individuales.
- El usuario realiza la conversión en dos pasos (`FacetaA` -> `Gemstone` -> `FacetaB`). Esto reduce el número de operadores de conversión a escribir a exactamente $2N$ (complejidad $O(N)$).

---

## Progresión de Complejidad

### Nivel Simple: Master Pointer de propiedad exclusiva
El master pointer crea su objeto en el constructor y lo destruye de forma exclusiva en su destructor, protegiendo al sistema de punteros huérfanos.

### Nivel Aplicado: Master Pointers con contadores de referencia
Para soportar copia y asignación complejas, se integran contadores de referencias para determinar de forma segura cuándo liberar el recurso físico del heap.

### Nivel Complejo: Punteros Invisibles (Invisible Pointers)
Para lograr un encapsulamiento absoluto, las clases físicas concretas (`ConcreteFoo`) se definen de forma exclusiva dentro del archivo de implementación `.cpp` de la biblioteca, sin cabecera pública asociada.
- El cliente externo trabaja únicamente con una interfaz abstracta pura (`Foo`) envuelta en un master pointer inteligente.
- La creación se realiza mediante factory functions estáticas (`Foo::make()`), asegurando que ningún detalle de implementación de la clase física se exponga al compilador en tiempo de compilación externa.

---

## Diseño de Sistemas
Estas abstracciones inspiraron estándares industriales de interoperabilidad binaria como **COM (Component Object Model)** de Microsoft, donde la llamada a `QueryInterface` actúa como el coordinador Gemstone para solicitar facetas específicas a partir de identificadores únicos.

---

## Ejercicios

### Ejercicio 1 — Simulación de compactación con Baker
Implementa un programa que simule un Heap simplificado de dos espacios. Crea una clase `Handle` que acceda a un objeto a través de una tabla centralizada de Master Pointers. Escribe una función de compactación que mueva el objeto de dirección en memoria física y demuestra que el cliente sigue leyendo su contenido de forma correcta usando el `Handle`.

---

## Errores Comunes y Anti-Patrones
- **Exponer el puntero crudo desde una faceta:** Permite que los clientes rompan el encapsulamiento de la faceta y accedan a métodos no publicados del objeto subyacente.
- **Ignorar el ciclo de vida del Master Pointer en Handles:** Provoca fugas de los objetos de control intermedio.

---

## Conclusión y Checklist Mental
- [ ] ¿Cómo ayuda un Handle a implementar desfragmentación de memoria en tiempo real?
- [ ] ¿Qué ventaja de tiempo de compilación aporta el uso de Facetas?
- [ ] ¿Cómo reduce la clase Gemstone la complejidad de conversión de facetas de $O(N^2)$ a $O(N)$?
- [ ] ¿Qué es un "Puntero Invisible" y cómo mejora el encapsulamiento absoluto de APIs?

---

*Siguiente tema sugerido: [32 — Templates en C++](<32 — Templates en C++.md>)*
