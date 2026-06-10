// =============================================================================
// block_scope_semantics.cpp
// Tema del Examen: P7 — Bloques, Ámbitos y Semántica Operacional
//
// Estructura:
//   [1] scope::basico          — bloque simple: δ se descarta al salir
//   [2] scope::efectos_ext     — sentencias dentro del bloque modifican γ exterior
//   [3] scope::shadowing       — variable local oculta (shadows) a la exterior
//   [4] scope::anidado         — bloques anidados: pila de marcos γ.δ₁.δ₂
//   [5] scope::ciclo_vida      — constructores/destructores marcan entrada/salida
//   [6] main()                 — demostración con salida esperada comentada
//
// Regla formal del bloque (Big-Step Semantics):
//
//   γ . ∅ ⊢ s₁ … sₙ ⟹ γ' . δ
//   ────────────────────────────────
//      γ ⊢ { s₁ … sₙ } ⟹ γ'
//
//   γ  = entorno EXTERIOR (ya existía antes del bloque)
//   ∅  = marco local vacío que se CREA al entrar
//   δ  = variables DECLARADAS dentro del bloque  (se DESCARTAN al salir)
//   γ' = entorno exterior MODIFICADO por las sentencias (PERSISTE al salir)
// =============================================================================

#include <iostream>
#include <string>

// =============================================================================
// [1] scope::basico
//     Muestra que las variables declaradas dentro del bloque (δ)
//     no existen fuera de él. El entorno exterior (γ) no las ve.
// =============================================================================
namespace scope::basico {

    void demo() {
        std::cout << "--- [1] basico ---\n";

        int x = 10;   // γ = {x:=10}

        {
            // Entramos al bloque: marco local ∅ se apila.
            int y = 20;       // δ = {y:=20}     (variable LOCAL al bloque)
            int z = x + y;    // δ += {z:=30}    (usa x del exterior)

            std::cout << "  dentro:  x=" << x << "  y=" << y << "  z=" << z << "\n";
            // dentro: x=10  y=20  z=30
        }
        // Salimos: δ={y,z} se DESTRUYE. Solo γ'={x:=10} sobrevive.

        std::cout << "  fuera:   x=" << x << "\n";  // x=10 (sin cambios)
        // 'y' y 'z' ya no existen; accederlos sería error de compilación.
    }

} // namespace scope::basico


// =============================================================================
// [2] scope::efectos_ext
//     Las sentencias dentro del bloque SÍ pueden modificar variables del
//     entorno exterior (γ → γ'). Esas modificaciones PERSISTEN al salir.
// =============================================================================
namespace scope::efectos_ext {

    void demo() {
        std::cout << "--- [2] efectos sobre el exterior ---\n";

        int x = 0;   // γ = {x:=0}

        {
            // Regla: el bloque se ejecuta en γ . ∅
            int y = 5;     // δ = {y:=5}
            x = x + y;    // Modifica γ: γ' = {x:=5}
                           // (x pertenece al exterior; el cambio persiste)
        }
        // Al salir: δ={y} se descarta. γ' = {x:=5} permanece.

        std::cout << "  x despues del bloque = " << x << "\n";  // 5

        // Premisa semántica:  {x:=0}.{} ⊢ int y=5; x=x+y; ⟹ {x:=5}.{y:=5}
        // Conclusión:         {x:=0} ⊢ { int y=5; x=x+y; } ⟹ {x:=5}
    }

} // namespace scope::efectos_ext


// =============================================================================
// [3] scope::shadowing
//     Una variable LOCAL puede tener el mismo nombre que una EXTERIOR.
//     Dentro del bloque, la local "tapa" (shadows) a la exterior.
//     Al salir, la exterior recupera su valor original — no fue modificada.
// =============================================================================
namespace scope::shadowing {

    void demo() {
        std::cout << "--- [3] shadowing (ocultamiento) ---\n";

        int x = 100;   // γ = {x:=100}  (exterior)

        std::cout << "  antes del bloque:  x = " << x << "\n";   // 100

        {
            // 'x' LOCAL: nueva variable que oculta a la exterior.
            // En términos formales: el marco local δ tiene su propio 'x'.
            // La búsqueda va desde la cima (δ) hacia la base (γ),
            // por eso la local tiene prioridad.
            int x = 999;   // δ = {x:=999}  ← distinto objeto, mismo nombre
            std::cout << "  dentro del bloque: x = " << x << "\n";   // 999

            // La 'x' del exterior no fue tocada.
        }
        // δ={x:=999} se descarta. La 'x' del exterior sigue intacta.

        std::cout << "  fuera del bloque:  x = " << x << "\n";   // 100
    }

} // namespace scope::shadowing


// =============================================================================
// [4] scope::anidado
//     Bloques dentro de bloques: la pila crece con cada bloque.
//     γ . δ₁ . δ₂ — al salir de cada bloque se desapila su marco.
// =============================================================================
namespace scope::anidado {

    void demo() {
        std::cout << "--- [4] bloques anidados ---\n";

        int a = 1;   // γ = {a:=1}

        {   // Nivel 1: γ . ∅
            int b = 2;   // δ₁ = {b:=2}
            a += b;      // γ' = {a:=3}

            {   // Nivel 2: γ' . δ₁ . ∅
                int c = 10;      // δ₂ = {c:=10}
                a += c;          // γ'' = {a:=13}
                b += c;          // modifica b en δ₁ → δ₁' = {b:=12}

                std::cout << "  nivel 2: a=" << a
                          << "  b=" << b << "  c=" << c << "\n";
                // nivel 2: a=13  b=12  c=10
            }
            // δ₂={c} se descarta.

            std::cout << "  nivel 1: a=" << a << "  b=" << b << "\n";
            // nivel 1: a=13  b=12   ('b' aún vive en δ₁)
        }
        // δ₁={b} se descarta.

        std::cout << "  exterior: a=" << a << "\n";   // a=13
        // Solo sobrevive el γ'' con a=13.
    }

} // namespace scope::anidado


// =============================================================================
// [5] scope::ciclo_vida
//     Los destructores de objetos locales evidencian físicamente el punto
//     exacto en que δ se descarta: justo al cerrar la llave '}'.
//     El orden de destrucción es INVERSO al de construcción (LIFO).
// =============================================================================
namespace scope::ciclo_vida {

    struct Recurso {
        std::string nombre;

        explicit Recurso(const std::string& n) : nombre(n) {
            std::cout << "  [ctor] " << nombre << "\n";
        }

        ~Recurso() {
            std::cout << "  [dtor] " << nombre << " ← δ descartado aquí\n";
        }
    };

    void demo() {
        std::cout << "--- [5] ciclo de vida (ctors/dtors) ---\n";

        Recurso r1("exterior");   // γ: r1 vive toda la función

        {
            Recurso r2("local_A");   // δ: r2 entra al bloque
            Recurso r3("local_B");   // δ: r3 entra al bloque
            std::cout << "  (dentro del bloque)\n";
            // Al cerrar '}': r3 se destruye primero (LIFO), luego r2.
        }
        // r3 y r2 ya murieron (δ descartado).

        std::cout << "  (fuera del bloque, r1 sigue vivo)\n";
        // r1 se destruye al salir de demo().
    }

} // namespace scope::ciclo_vida


// =============================================================================
// main
// =============================================================================
int main() {
    std::cout << "=== Bloques y Alcance — Semántica Operacional (P7) ===\n\n";

    scope::basico::demo();
    std::cout << "\n";

    scope::efectos_ext::demo();
    std::cout << "\n";

    scope::shadowing::demo();
    std::cout << "\n";

    scope::anidado::demo();
    std::cout << "\n";

    scope::ciclo_vida::demo();

    return 0;
}
