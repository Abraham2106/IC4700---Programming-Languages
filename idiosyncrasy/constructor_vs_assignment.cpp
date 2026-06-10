// =============================================================================
// constructor_vs_assignment.cpp
// Temas del Examen: P2 y P4
//
// Estructura:
//   [1] ctor_vs_assign::basico      — constructor vs operator= (P2)
//   [2] ctor_vs_assign::explicit_kw — efecto de 'explicit' en conversiones
//   [3] ctor_vs_assign::errores     — tipo de retorno ilegal + visibilidad (P4)
//   [4] ctor_vs_assign::corregido   — versión sin errores de P4
//   [5] main()                      — demostración comentada
//
// Regla de oro:
//   '=' en una DECLARACIÓN  → llama al CONSTRUCTOR  (el objeto aún no existe)
//   '=' sobre objeto VIVO   → llama a OPERATOR=      (el objeto ya existe)
// =============================================================================

#include <iostream>
#include <string>

// =============================================================================
// [1] ctor_vs_assign::basico
//     Demuestra que 'A b = 5' es una inicialización (constructor), NO operator=
// =============================================================================
namespace ctor_vs_assign::basico {

    class A {
        int n;
    public:
        // Constructor con argumento
        A(int a) : n(a) {
            std::cout << "  [Constructor A(int)]  n = " << a << "\n";
        }

        // Operador de asignación desde int
        A& operator=(int a) {
            std::cout << "  [operator=(int)]       n = " << a << "\n";
            n = a;
            return *this;
        }

        int get() const { return n; }
    };

    void demo() {
        std::cout << "--- [1] basico ---\n";

        // (1) Inicialización DIRECTA → Constructor A(int)
        A a(3);
        // Imprime: [Constructor A(int)] n = 3

        // (2) Inicialización POR COPIA → Constructor A(int), NO operator=
        //     'b' no existe todavía: el '=' aquí es sintaxis de inicialización.
        //     El compilador convierte implícitamente '5' a A(5).
        A b = 5;
        // Imprime: [Constructor A(int)] n = 5

        // (3) ASIGNACIÓN sobre objeto existente → operator=(int)
        //     'a' ya está construido: el '=' invoca el operador de asignación.
        a = 7;
        // Imprime: [operator=(int)] n = 7

        std::cout << "a.get() = " << a.get() << "\n"; // 7
        std::cout << "b.get() = " << b.get() << "\n"; // 5
    }

} // namespace ctor_vs_assign::basico


// =============================================================================
// [2] ctor_vs_assign::explicit_kw
//     Muestra el efecto de marcar el constructor como 'explicit'.
//     Con 'explicit', la conversión implícita (A b = 5) deja de compilar.
// =============================================================================
namespace ctor_vs_assign::explicit_kw {

    class B {
        int n;
    public:
        // 'explicit' prohíbe conversiones implícitas en inicializaciones
        explicit B(int a) : n(a) {
            std::cout << "  [Constructor explicit B(int)]  n = " << a << "\n";
        }

        int get() const { return n; }
    };

    void demo() {
        std::cout << "--- [2] explicit ---\n";

        // B b = 5;         // ❌ ERROR si se descomenta:
        //                  //    "conversión implícita a tipo 'B' no permitida"

        B b(5);             // ✅ Inicialización directa — siempre funciona
        B c = B(10);        // ✅ Conversión explícita manual — también válida

        std::cout << "b.get() = " << b.get() << "\n"; // 5
        std::cout << "c.get() = " << c.get() << "\n"; // 10
    }

} // namespace ctor_vs_assign::explicit_kw


// =============================================================================
// [3] ctor_vs_assign::errores
//     Muestra el código PROBLEMÁTICO del examen (P4) — NO COMPILA.
//     Los errores están documentados con comentarios; descoméntalos para verlos.
//
//     Tres problemas:
//       E1: Los constructores NO pueden tener tipo de retorno.
//       E2: Miembros de 'class' son 'private' por defecto →
//           constructores privados → no se pueden crear objetos desde fuera.
//       E3: 'Foo' no tiene constructor por defecto (solo tiene Foo(int)),
//           así que 'Bar()' no puede inicializar la base automáticamente.
// =============================================================================
namespace ctor_vs_assign::errores {

    // ── Código del examen (tal cual) ──────────────────────────────────────
    //
    // class Foo {
    //     Foo& Foo(int i) { }          // ❌ E1: tipo de retorno 'Foo&' inválido
    //                                  // ❌ E2: privado por defecto
    // };
    //
    // class Bar : public Foo {
    //     Bar& Bar() { }               // ❌ E1: tipo de retorno 'Bar&' inválido
    //                                  // ❌ E2: privado por defecto
    //                                  // ❌ E3: busca Foo() inexistente
    // };
    // ─────────────────────────────────────────────────────────────────────

    // Clase vacía de marcador solo para que el namespace compile
    struct Marcador {};

    void demo() {
        std::cout << "--- [3] errores (codigo del examen) ---\n";
        std::cout << "  El codigo del examen tiene 3 errores de compilacion:\n";
        std::cout << "  E1: Los constructores no pueden tener tipo de retorno.\n";
        std::cout << "  E2: Miembros de 'class' son private por defecto.\n";
        std::cout << "  E3: Bar necesita un Foo() inexistente al construirse.\n";
    }

} // namespace ctor_vs_assign::errores


// =============================================================================
// [4] ctor_vs_assign::corregido
//     Versión corregida del código del examen (P4).
//     Aplica las tres correcciones:
//       C1: Quitar el tipo de retorno de los constructores.
//       C2: Agregar 'public:' antes de los constructores.
//       C3: Llamar a Foo(int) en la lista de inicialización de Bar.
// =============================================================================
namespace ctor_vs_assign::corregido {

    class Foo {
    public:
        int valor;

        // C1: Sin tipo de retorno   C2: Declarado public
        Foo(int i) : valor(i) {
            std::cout << "  [Foo(int)]  valor = " << i << "\n";
        }
        // Si se quisiera también un constructor por defecto:
        // Foo() : valor(0) {}
    };

    class Bar : public Foo {
    public:
        std::string nombre;

        // C1: Sin tipo de retorno   C2: Declarado public
        // C3: Lista de inicialización invoca Foo(0) — único constructor disponible
        Bar() : Foo(0), nombre("bar_default") {
            std::cout << "  [Bar()]  base.valor = " << valor << "\n";
        }

        Bar(int x, const std::string& n) : Foo(x), nombre(n) {
            std::cout << "  [Bar(int,string)]  base.valor = " << x
                      << "  nombre = " << n << "\n";
        }
    };

    void demo() {
        std::cout << "--- [4] corregido ---\n";
        Bar b1;                  // Foo(0) → Bar()
        Bar b2(42, "ejemplo");   // Foo(42) → Bar(42, "ejemplo")

        std::cout << "b1.nombre = " << b1.nombre << "\n";
        std::cout << "b2.nombre = " << b2.nombre << "\n";
    }

} // namespace ctor_vs_assign::corregido


// =============================================================================
// main
// =============================================================================
int main() {
    std::cout << "=== Constructor vs operator= (P2 y P4) ===\n\n";

    ctor_vs_assign::basico::demo();
    std::cout << "\n";

    ctor_vs_assign::explicit_kw::demo();
    std::cout << "\n";

    ctor_vs_assign::errores::demo();
    std::cout << "\n";

    ctor_vs_assign::corregido::demo();

    return 0;
}
