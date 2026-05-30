# Smart Pointers, Handles y Facets — Patrones avanzados de encapsulación de recursos e indirección

Los punteros parametrizados y los patrones de handles aíslan la implementación física del recurso del acceso del cliente. Estas técnicas reducen el acoplamiento y facilitan la optimización interna del almacenamiento sin alterar la API expuesta.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
- **Manejador (Handle):** Es un puntero indirecto (puntero a puntero). En lugar de apuntar directamente al objeto destino, el Handle apunta a un *Master Pointer*. Esto permite mover el objeto destino en memoria física (para compactación o reubicación) actualizando una única dirección de memoria intermedia, sin invalidar los punteros de los clientes.
- **Faceta (Facet):** Es un smart pointer especializado que expone únicamente un subconjunto de la interfaz total de un objeto pesado (por ejemplo, separar las funciones de dibujo de las funciones de eventos en una clase `View`).
- **Gemstone (Piedra preciosa):** Un objeto coordinador que centraliza la administración y conversión entre múltiples facetas para un mismo objeto subyacente.

### 1.2 ¿Por qué importa?
- **Compactación de Memoria:** Permite reubicar dinámicamente objetos para consolidar el espacio libre del Heap (desfragmentación) sin romper las referencias de los clientes.
- **Factorización de Interfaces:** En clases complejas con docenas de métodos (como controles de interfaz gráfica), las facetas evitan compilar interfaces gigantescas, reduciendo dependencias en el tiempo de compilación.

---

## 2. Hook Example

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

## 3. Descomposición Under the Hood

### 3.1 Compactación física usando Handles y el Algoritmo de Baker
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

## 4. Teoría: Handles, Facets y Gemstones

### 4.1 1. Desacoplamiento mediante Facetas (Facets)
Cuando una clase (como un control de ventana `View`) tiene más de 80 funciones miembro, compilar sus cabeceras expone demasiados detalles lógicos internos.
- Se crean facetas (`ViewDrawing`, `ViewEvents`, `ViewGeometry`) que replican solo un subconjunto de funciones y delegan internamente al objeto real.
- El código cliente que solo dibuja solo incluye `ViewDrawing.h`, evitando recompilar si cambian los eventos de entrada de la ventana.

### 4.2 2. Escalabilidad con Gemstones
Si tienes $N$ facetas y deseas permitir que el usuario obtenga una faceta a partir de otra de forma directa (por ejemplo, obtener la interfaz de eventos desde la de dibujo), escribir conversiones directas entre todas requiere programar $N(N-1)$ operadores de conversión (complejidad $O(N^2)$).
- **Solución Gemstone:** Cada faceta implementa una única conversión a un tipo común `Gemstone`. El `Gemstone` a su vez tiene conversiones a todas las facetas individuales.
- El usuario realiza la conversión en dos pasos (`FacetaA` -> `Gemstone` -> `FacetaB`). Esto reduce el número de operadores de conversión a escribir a exactamente $2N$ (complejidad $O(N)$).

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Master Pointer de propiedad exclusiva
El master pointer crea su objeto en el constructor y lo destruye de forma exclusiva en su destructor, protegiendo al sistema de punteros huérfanos.

### 5.2 Nivel Aplicado: Master Pointers con contadores de referencia
Para soportar copia y asignación complejas, se integran contadores de referencias para determinar de forma segura cuándo liberar el recurso físico del heap.

### 5.3 Nivel Complejo: Punteros Invisibles (Invisible Pointers)
Para lograr un encapsulamiento absoluto, las clases físicas concretas (`ConcreteFoo`) se definen de forma exclusiva dentro del archivo de implementación `.cpp` de la biblioteca, sin cabecera pública asociada.
- El cliente externo trabaja únicamente con una interfaz abstracta pura (`Foo`) envuelta en un master pointer inteligente.
- La creación se realiza mediante factory functions estáticas (`Foo::make()`), asegurando que ningún detalle de implementación de la clase física se exponga al compilador en tiempo de compilación externa.

---

## 6. Diseño de Sistemas
Estas abstracciones inspiraron estándares industriales de interoperabilidad binaria como **COM (Component Object Model)** de Microsoft, donde la llamada a `QueryInterface` actúa como el coordinador Gemstone para solicitar facetas específicas a partir de identificadores únicos.

---

## Exercises

### Exercise 1 — Simulación de compactación con Baker
Implementa un programa que simule un Heap simplificado de dos espacios. Crea una clase `Handle` que acceda a un objeto a través de una tabla centralizada de Master Pointers. Escribe una función de compactación que mueva el objeto de dirección en memoria física y demuestra que el cliente sigue leyendo su contenido de forma correcta usando el `Handle`.

---

## 7. Errores Comunes y Anti-Patrones
- **Exponer el puntero crudo desde una faceta:** Permite que los clientes rompan el encapsulamiento de la faceta y accedan a métodos no publicados del objeto subyacente.
- **Ignorar el ciclo de vida del Master Pointer en Handles:** Provoca fugas de los objetos de control intermedio.

---

## 8. Conclusión

---

---

*Next: `32 — Templates en C++.md` — Programación genérica en tiempo de compilación.*
