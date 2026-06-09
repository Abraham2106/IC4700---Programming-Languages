// =============================================================================
// NOTA PREVIA — ¿Qué es un namespace y cómo leer lsp::violacion::rectangulo?
// -----------------------------------------------------------------------------
// Un namespace es un contenedor lógico que agrupa código bajo un nombre.
// Evita colisiones: dos clases "Rectangulo" pueden coexistir si viven en
// namespaces distintos, igual que dos archivos con el mismo nombre pueden
// coexistir en carpetas distintas.
//
// El operador "::" se lee como "dentro de" o "perteneciente a":
//
//   lsp :: violacion :: rectangulo
//    │          │            └─ sub-namespace: el caso concreto (rectangulo)
//    │          └─ sub-namespace: la intención (violacion | solucion)
//    └─ namespace raíz: el tema general (lsp)
//
// Entonces:
//   lsp::violacion::rectangulo  →  "dentro de LSP, en la sección de
//                                   violaciones, el caso del rectángulo"
//   lsp::solucion::rectangulo   →  "dentro de LSP, en la sección de
//                                   soluciones, el caso del rectángulo"
//
// Esta estructura nos permite tener una clase "Rectangulo" en la violación
// y otra clase "Rectangulo" (corregida) en la solución, sin que C++ se
// confunda entre ellas. Cada namespace es su propio universo de nombres.
// =============================================================================

// =============================================================================
// lsp_principio_liskov.cpp
// Principio de Sustitución de Liskov (LSP) — Ejemplos en profundidad
//
// Estructura del archivo:
//   [1] lsp::violacion::rectangulo   — Anti-patrón Rectángulo/Cuadrado
//   [2] lsp::solucion::rectangulo    — Corrección via interfaz común inmutable
//   [3] lsp::violacion::coleccion    — Precondiciones endurecidas (VectorPositivos)
//   [4] lsp::solucion::coleccion     — Corrección via composición
//   [5] lsp::violacion::aves         — Postcondición rota (Pinguino no vuela)
//   [6] lsp::solucion::aves          — Segregación de capacidades
//   [7] main()                       — Demostración de cada caso
//
// Regla de oro de Liskov:
//   - Precondiciones : la derivada NO puede pedir MÁS que la base.
//   - Postcondiciones: la derivada NO puede entregar MENOS que la base.
//   - Invariantes    : los estados lógicos prometidos deben preservarse.
//   - Excepciones    : la derivada NO puede lanzar lo que el cliente no espera.
// =============================================================================

#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
#include <memory>

// =============================================================================
// [1] lsp::violacion::rectangulo
//     Anti-patrón clásico: Cuadrado hereda de Rectángulo y rompe el
//     invariante de independencia de ejes.
// =============================================================================
// =============================================================================
// [1] lsp::violacion::rectangulo
// =============================================================================
namespace lsp::violacion::rectangulo {

    // La base promete que ancho y alto son INDEPENDIENTES.
    // El cliente puede modificar uno sin que el otro cambie.
    class Rectangulo {
    protected:
        int ancho, alto;
    public:
        Rectangulo(int a, int h) : ancho(a), alto(h) {}

        virtual void set_ancho(int a) { ancho = a; }
        virtual void set_alto(int h)  { alto  = h; }

        int get_ancho() const { return ancho; }
        int get_alto()  const { return alto;  }
        int get_area()  const { return ancho * alto; }
    };

    // Matemáticamente un cuadrado ES un rectángulo.
    // Pero al heredar y forzar ancho == alto en los setters,
    // rompemos el invariante de independencia de ejes que la base prometía.
    class Cuadrado : public Rectangulo {
    public:
        Cuadrado(int lado) : Rectangulo(lado, lado) {}

        // VIOLACIÓN: modificar uno fuerza al otro — el cliente no lo espera.
        void set_ancho(int a) override { ancho = alto = a; }
        void set_alto(int h)  override { ancho = alto = h; }
    };

    // Función cliente — escrita 100% contra la base (Rectangulo&).
    // Asume el invariante: set_ancho no toca el alto, y viceversa.
    void redimensionar(Rectangulo& r) {
        r.set_ancho(5);
        r.set_alto(10);
        // Esperado siempre: 5 * 10 = 50
        // Si r es un Cuadrado: 10 * 10 = 100  ← LSP roto
        std::cout << "Area: " << r.get_area()
                  << (r.get_area() == 50 ? "  [OK]" : "  [VIOLACION LSP]")
                  << "\n";
    }

} // namespace lsp::violacion::rectangulo


// =============================================================================
// [2] lsp::solucion::rectangulo
//     Fix: extraer una abstracción común (FiguraGeometrica) sin setters
//     mutables; cuadrado y rectángulo son hojas independientes.
// =============================================================================
namespace lsp::solucion::rectangulo {

    // Fix: extraemos una abstracción común SIN setters mutables.
    // FiguraGeometrica solo promete lo que ambas figuras SÍ comparten:
    // tener un área calculable. Nada más, nada menos.
    class FiguraGeometrica {
    public:
        virtual int get_area() const = 0;
        virtual ~FiguraGeometrica() = default;
    };

    // Rectangulo es una hoja independiente.
    // Sus setters son suyos — no los hereda nadie, no rompen nada.
    class Rectangulo : public FiguraGeometrica {
        int ancho, alto;
    public:
        Rectangulo(int a, int h) : ancho(a), alto(h) {}

        void set_ancho(int a) { ancho = a; }
        void set_alto(int h)  { alto  = h; }

        int get_area() const override { return ancho * alto; }
    };

    // Cuadrado es también una hoja independiente.
    // Gestiona su invariante (ancho == alto) internamente,
    // sin interferir con el contrato de nadie más.
    class Cuadrado : public FiguraGeometrica {
        int lado;
    public:
        Cuadrado(int l) : lado(l) {}

        void set_lado(int l) { lado = l; }

        int get_area() const override { return lado * lado; }
    };

    // El cliente ahora opera sobre FiguraGeometrica& —
    // solo exige lo que la abstracción SÍ garantiza: un área.
    // No asume nada sobre setters ni ejes. LSP preservado.
    void imprimir_area(const FiguraGeometrica& f) {
        std::cout << "Area: " << f.get_area() << "  [OK]\n";
    }

} // namespace lsp::solucion::rectangulo


// =============================================================================
// [3] lsp::violacion::coleccion
//     Anti-patrón: derivada endurece precondiciones — solo acepta positivos,
//     rompiendo el contrato de la base que acepta cualquier entero.
// =============================================================================
namespace lsp::violacion::coleccion {

    // TODO: implementar en el hilo

} // namespace lsp::violacion::coleccion


// =============================================================================
// [4] lsp::solucion::coleccion
//     Fix: composición en lugar de herencia pública; el filtro es una
//     responsabilidad separada, no una restricción sobre el contrato base.
// =============================================================================
namespace lsp::solucion::coleccion {

    // TODO: implementar en el hilo

} // namespace lsp::solucion::coleccion


// =============================================================================
// [5] lsp::violacion::aves
//     Anti-patrón: Pinguino hereda de Ave (que promete volar) y lanza
//     una excepción en vuelo — postcondición rota.
// =============================================================================
namespace lsp::violacion::aves {

    // TODO: implementar en el hilo

} // namespace lsp::violacion::aves


// =============================================================================
// [6] lsp::solucion::aves
//     Fix: segregar la capacidad de vuelo en una interfaz aparte; Pinguino
//     es un Ave pero no un AveVoladora.
// =============================================================================
namespace lsp::solucion::aves {

    // TODO: implementar en el hilo

} // namespace lsp::solucion::aves


// =============================================================================
// main — Punto de entrada; cada sección se activa conforme avanza el hilo
// =============================================================================
int main() {

    std::cout << "=== LSP: Principio de Sustitución de Liskov ===\n\n";

    // [1] Violación Rectángulo/Cuadrado
    // [2] Solución Rectángulo/Cuadrado
    // [3] Violación Colección
    // [4] Solución Colección
    // [5] Violación Aves
    // [6] Solución Aves

    return 0;
}