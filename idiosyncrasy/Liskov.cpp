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

    // La base promete aceptar CUALQUIER entero.
    class VectorBase {
    protected:
        std::vector<int> datos;
    public:
        virtual void agregar(int n) {
            datos.push_back(n);
        }
        const std::vector<int>& get() const { return datos; }
    };

    // VIOLACIÓN: la derivada ENDURECE la precondición — rechaza negativos.
    // Un cliente que use un VectorBase* puede pasar -1 sin problema.
    // Si en realidad tiene un VectorSoloPositivos, explota con una excepción
    // que no esperaba: el subtipo NO es sustituible.
    class VectorSoloPositivos : public VectorBase {
    public:
        void agregar(int n) override {
            if (n < 0)
                throw std::invalid_argument("Solo se permiten enteros positivos");
            datos.push_back(n);
        }
    };

    // Cliente escrito contra la interfaz BASE — no sabe qué tipo real recibe.
    void llenar(VectorBase& v) {
        v.agregar(10);
        v.agregar(-5);   // ← espera que esto funcione; con VectorBase sí.
                          //   con VectorSoloPositivos → excepción inesperada.
    }

    void demo() {
        std::cout << "--- [3] VectorSoloPositivos (violacion) ---\n";

        VectorBase base;
        llenar(base);    // OK: base acepta -5 sin problema
        std::cout << "  VectorBase: ";
        for (int n : base.get()) std::cout << n << " ";
        std::cout << "\n";

        VectorSoloPositivos restringido;
        try {
            llenar(restringido);   // FALLA: -5 lanza excepcion
        } catch (const std::invalid_argument& e) {
            std::cout << "  VectorSoloPositivos: excepcion ← [VIOLACION LSP]: "
                      << e.what() << "\n";
        }
    }

} // namespace lsp::violacion::coleccion


// =============================================================================
// [4] lsp::solucion::coleccion
//     Fix: composición en lugar de herencia pública; el filtro es una
//     responsabilidad separada, no una restricción sobre el contrato base.
// =============================================================================
namespace lsp::solucion::coleccion {

    // Fix: el filtro es una RESPONSABILIDAD SEPARADA, no una restricción
    // sobre el contrato base. Usamos COMPOSICIÓN en lugar de herencia pública.
    //
    // VectorBase sigue aceptando cualquier entero — su contrato no cambia.
    class VectorBase {
        std::vector<int> datos;
    public:
        void agregar(int n) { datos.push_back(n); }
        const std::vector<int>& get() const { return datos; }
    };

    // VectorFiltrado CONTIENE un VectorBase; no hereda de él.
    // No hace promesas de sustituibilidad: es una clase independiente.
    class VectorFiltrado {
        VectorBase interno;
    public:
        // Responsabilidad propia: filtrar. No viola ningún contrato ajeno.
        void agregar_si_positivo(int n) {
            if (n >= 0) interno.agregar(n);
            // Simplemente ignora los negativos; no lanza excepción inesperada.
        }
        const std::vector<int>& get() const { return interno.get(); }
    };

    void demo() {
        std::cout << "--- [4] VectorFiltrado (solucion por composicion) ---\n";

        VectorBase base;
        base.agregar(10);
        base.agregar(-5);   // OK: VectorBase siempre acepta todo
        std::cout << "  VectorBase: ";
        for (int n : base.get()) std::cout << n << " ";
        std::cout << "\n";  // 10 -5

        VectorFiltrado filtrado;
        filtrado.agregar_si_positivo(10);
        filtrado.agregar_si_positivo(-5);  // ignorado silenciosamente
        filtrado.agregar_si_positivo(7);
        std::cout << "  VectorFiltrado: ";
        for (int n : filtrado.get()) std::cout << n << " ";
        std::cout << "  [OK]\n";  // 10 7
    }

} // namespace lsp::solucion::coleccion


// =============================================================================
// [5] lsp::violacion::aves
//     Anti-patrón: Pinguino hereda de Ave (que promete volar) y lanza
//     una excepción en vuelo — postcondición rota.
// =============================================================================
namespace lsp::violacion::aves {

    // La base Ave promete la postcondición: volar() siempre funciona.
    class Ave {
    public:
        virtual std::string volar() {
            return "Estoy volando";
        }
        virtual std::string nombre() const { return "Ave"; }
        virtual ~Ave() = default;
    };

    // VIOLACIÓN: Pinguino hereda de Ave pero ROMPE la postcondición:
    // volar() lanza una excepción que el cliente no espera.
    class Pinguino : public Ave {
    public:
        std::string volar() override {
            throw std::logic_error("Los pinguinos no pueden volar");
        }
        std::string nombre() const override { return "Pinguino"; }
    };

    // Cliente escrito contra la interfaz Ave — asume que volar() siempre
    // devuelve un string. Con Pinguino explota.
    void hacer_volar(Ave& ave) {
        std::cout << "  " << ave.nombre() << ": " << ave.volar() << "\n";
    }

    void demo() {
        std::cout << "--- [5] Pinguino no puede volar (violacion) ---\n";

        Ave golondrina;
        hacer_volar(golondrina);   // OK

        Pinguino tux;
        try {
            hacer_volar(tux);      // FALLA: postcondicion rota
        } catch (const std::logic_error& e) {
            std::cout << "  Pinguino: excepcion ← [VIOLACION LSP]: "
                      << e.what() << "\n";
        }
    }

} // namespace lsp::violacion::aves


// =============================================================================
// [6] lsp::solucion::aves
//     Fix: segregar la capacidad de vuelo en una interfaz aparte; Pinguino
//     es un Ave pero no un AveVoladora.
// =============================================================================
namespace lsp::solucion::aves {

    // Fix: segregar la capacidad de vuelo en una interfaz aparte.
    // Ave solo promete lo que TODOS los aves comparten.
    // AveVoladora extiende con la promesa de vuelo.
    //
    // Este patrón es también el Principio de Segregación de Interfaces (ISP),
    // la 'I' de SOLID — evitar interfaces «gordas» que no todos pueden cumplir.

    class Ave {
    public:
        virtual std::string nombre() const = 0;
        virtual std::string describir() const { return "Soy un ave"; }
        virtual ~Ave() = default;
    };

    // Capacidad de vuelo segregada: solo quien PUEDE volar la implementa.
    class AveVoladora : public Ave {
    public:
        virtual std::string volar() = 0;
    };

    // Golondrina SÍ puede volar — hereda de AveVoladora.
    class Golondrina : public AveVoladora {
    public:
        std::string nombre() const override { return "Golondrina"; }
        std::string volar() override { return "Volando veloz sobre los campos"; }
    };

    // Pinguino NO puede volar — hereda de Ave directamente.
    // No hace ninguna promesa de vuelo. LSP preservado.
    class Pinguino : public Ave {
    public:
        std::string nombre() const override { return "Pinguino"; }
        std::string nadar() const { return "Nadando en aguas heladas"; }
    };

    // Cliente de AveVoladora — solo lo llama con objetos que sí vuelan.
    void hacer_volar(AveVoladora& ave) {
        std::cout << "  " << ave.nombre() << ": " << ave.volar() << "  [OK]\n";
    }

    // Cliente de Ave — solo usa la interfaz base (describir/nombre).
    void presentar(const Ave& ave) {
        std::cout << "  " << ave.nombre() << ": " << ave.describir() << "\n";
    }

    void demo() {
        std::cout << "--- [6] Segregacion de interfaz (solucion) ---\n";

        Golondrina g;
        Pinguino p;

        // Todos son Aves
        presentar(g);
        presentar(p);

        // Solo la Golondrina vuela
        hacer_volar(g);
        // hacer_volar(p);  ← Error de compilacion: Pinguino no es AveVoladora
        std::cout << "  " << p.nombre() << ": " << p.nadar() << "\n";
    }

} // namespace lsp::solucion::aves


// =============================================================================
// main — Punto de entrada; cada sección se activa conforme avanza el hilo
// =============================================================================
int main() {

    std::cout << "=== LSP: Principio de Sustitución de Liskov ===\n\n";

    // [1] Violación Rectángulo/Cuadrado
    {
        lsp::violacion::rectangulo::Rectangulo rect(3, 4);
        lsp::violacion::rectangulo::Cuadrado   cuad(5);
        std::cout << "Rectangulo (esperado 50): ";
        lsp::violacion::rectangulo::redimensionar(rect);  // 50 [OK]
        std::cout << "Cuadrado   (esperado 50): ";
        lsp::violacion::rectangulo::redimensionar(cuad);  // 100 [VIOLACION LSP]
    }
    std::cout << "\n";

    // [2] Solución Rectángulo/Cuadrado
    {
        lsp::solucion::rectangulo::Rectangulo rect(3, 4);
        lsp::solucion::rectangulo::Cuadrado   cuad(5);
        lsp::solucion::rectangulo::imprimir_area(rect);   // 12 [OK]
        lsp::solucion::rectangulo::imprimir_area(cuad);   // 25 [OK]
    }
    std::cout << "\n";

    // [3] Violación Colección
    lsp::violacion::coleccion::demo();
    std::cout << "\n";

    // [4] Solución Colección
    lsp::solucion::coleccion::demo();
    std::cout << "\n";

    // [5] Violación Aves
    lsp::violacion::aves::demo();
    std::cout << "\n";

    // [6] Solución Aves
    lsp::solucion::aves::demo();

    return 0;
}