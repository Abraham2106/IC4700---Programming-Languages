// =============================================================================
// increment_semantics.cpp
// Tema del Examen: P6 — Pre/Post Incremento y Decremento
//
// Estructura:
//   [1] incr::reglas        — las 4 operaciones aisladas (++x, x++, --x, x--)
//   [2] incr::examen_a      — ++x - x--  partiendo de x=1  → resultado: 0
//   [3] incr::examen_b      — x-- - --x  partiendo de x=1  → resultado: 2
//   [4] incr::extra         — casos adicionales para practicar
//   [5] main()              — salida comentada con valores esperados
//
// Regla formal (semántica operacional Big-Step):
//
//   γ ⊢ ++x ⟹ ⟨v+1, γ(x:=v+1)⟩   ← retorna el valor NUEVO, almacena nuevo
//   γ ⊢ x++ ⟹ ⟨v,   γ(x:=v+1)⟩   ← retorna el valor VIEJO, almacena nuevo
//   γ ⊢ --x ⟹ ⟨v-1, γ(x:=v-1)⟩   ← retorna el valor NUEVO, almacena nuevo
//   γ ⊢ x-- ⟹ ⟨v,   γ(x:=v-1)⟩   ← retorna el valor VIEJO, almacena nuevo
//
// La evaluación encadena estados de IZQUIERDA a DERECHA:
//   el γ' que produce la subexpresión izquierda ES el γ de entrada de la derecha.
// =============================================================================

#include <iostream>

// =============================================================================
// [1] incr::reglas — cada operación de forma aislada
// =============================================================================
namespace incr::reglas {

    void demo() {
        std::cout << "--- [1] reglas aisladas ---\n";

        // ── ++x (pre-incremento) ──────────────────────────────────────────
        // Incrementa PRIMERO, retorna el valor NUEVO.
        {
            int x = 5;
            int r = ++x;   // x pasa a 6, r recibe 6
            std::cout << "x=5 | r = ++x  → r=" << r << "  x=" << x << "\n";
            //          r=6, x=6
        }

        // ── x++ (post-incremento) ─────────────────────────────────────────
        // Retorna el valor VIEJO, incrementa DESPUÉS.
        {
            int x = 5;
            int r = x++;   // r recibe 5 (viejo), x pasa a 6
            std::cout << "x=5 | r = x++  → r=" << r << "  x=" << x << "\n";
            //          r=5, x=6
        }

        // ── --x (pre-decremento) ──────────────────────────────────────────
        // Decrementa PRIMERO, retorna el valor NUEVO.
        {
            int x = 5;
            int r = --x;   // x pasa a 4, r recibe 4
            std::cout << "x=5 | r = --x  → r=" << r << "  x=" << x << "\n";
            //          r=4, x=4
        }

        // ── x-- (post-decremento) ─────────────────────────────────────────
        // Retorna el valor VIEJO, decrementa DESPUÉS.
        {
            int x = 5;
            int r = x--;   // r recibe 5 (viejo), x pasa a 4
            std::cout << "x=5 | r = x--  → r=" << r << "  x=" << x << "\n";
            //          r=5, x=4
        }
    }

} // namespace incr::reglas


// =============================================================================
// [2] incr::examen_a — Pregunta a) del examen
//     Expresión: ++x - x--   con x inicial = 1
//
//     Traza (izquierda → derecha, encadenando γ):
//       γ₀ = {x:=1}
//       ++x  → retorna 2,  γ₁ = {x:=2}
//       x--  → retorna 2,  γ₂ = {x:=1}   (usa γ₁, retorna viejo valor: 2)
//       resultado = 2 - 2 = 0
//       x final  = 1
// =============================================================================
namespace incr::examen_a {

    void demo() {
        std::cout << "--- [2] examen a): ++x - x-- con x=1 ---\n";

        int x = 1;

        // Evaluación izquierda a derecha:
        //   subexpr izquierda: ++x  →  retorna 2, x queda en 2
        //   subexpr derecha:   x--  →  retorna 2 (el viejo), x queda en 1
        //   resta: 2 - 2 = 0
        int resultado = ++x - x--;

        std::cout << "resultado = " << resultado << "\n";  // 0
        std::cout << "x final   = " << x         << "\n";  // 1

        // Verificación explícita
        if (resultado == 0 && x == 1)
            std::cout << "✅ Correcto\n";
        else
            std::cout << "❌ Inesperado\n";
    }

} // namespace incr::examen_a


// =============================================================================
// [3] incr::examen_b — Pregunta b) del examen
//     Expresión: x-- - --x   con x inicial = 1
//
//     Traza (izquierda → derecha, encadenando γ):
//       γ₀ = {x:=1}
//       x--  → retorna 1 (viejo),  γ₁ = {x:=0}
//       --x  → retorna -1 (nuevo), γ₂ = {x:=-1}  (usa γ₁)
//       resultado = 1 - (-1) = 2
//       x final  = -1
// =============================================================================
namespace incr::examen_b {

    void demo() {
        std::cout << "--- [3] examen b): x-- - --x con x=1 ---\n";

        int x = 1;

        // Evaluación izquierda a derecha:
        //   subexpr izquierda: x--  →  retorna 1 (viejo), x queda en 0
        //   subexpr derecha:   --x  →  retorna -1 (nuevo), x queda en -1
        //   resta: 1 - (-1) = 2
        int resultado = x-- - --x;

        std::cout << "resultado = " << resultado << "\n";  // 2
        std::cout << "x final   = " << x         << "\n";  // -1

        // Verificación explícita
        if (resultado == 2 && x == -1)
            std::cout << "✅ Correcto\n";
        else
            std::cout << "❌ Inesperado\n";
    }

} // namespace incr::examen_b


// =============================================================================
// [4] incr::extra — casos adicionales para practicar
//     Practica las mismas reglas con variaciones.
// =============================================================================
namespace incr::extra {

    void demo() {
        std::cout << "--- [4] extra ---\n";

        // Caso: x++ + ++x  con x=3
        // Traza:
        //   x++  → retorna 3 (viejo), x queda en 4
        //   ++x  → retorna 5 (nuevo), x queda en 5
        //   suma: 3 + 5 = 8,  x final = 5
        {
            int x = 3;
            int r = x++ + ++x;
            std::cout << "x=3 | x++ + ++x  → resultado=" << r
                      << "  x=" << x << "  (esperado: 8, x=5)\n";
        }

        // Caso: --x + x++  con x=10
        // Traza:
        //   --x  → retorna 9 (nuevo), x queda en 9
        //   x++  → retorna 9 (viejo), x queda en 10
        //   suma: 9 + 9 = 18,  x final = 10
        {
            int x = 10;
            int r = --x + x++;
            std::cout << "x=10 | --x + x++  → resultado=" << r
                      << "  x=" << x << "  (esperado: 18, x=10)\n";
        }

        // Caso: x++ en una condición while
        // Itera mientras x++ < 3: evalúa el viejo x, luego incrementa.
        // Con x=0: evalúa 0,1,2 (todos < 3) → 3 iteraciones; x queda en 3.
        {
            int x = 0;
            int iter = 0;
            while (x++ < 3) iter++;
            std::cout << "while(x++<3) con x=0  → iter=" << iter
                      << "  x=" << x << "  (esperado: iter=3, x=4)\n";
            // Nota: la última evaluación de x++ es 3 (viejo), x pasa a 4.
            // La condición 3 < 3 es falsa → sale. x queda en 4.
        }
    }

} // namespace incr::extra


// =============================================================================
// main
// =============================================================================
int main() {
    std::cout << "=== Pre/Post Incremento — Semántica Operacional (P6) ===\n\n";

    incr::reglas::demo();
    std::cout << "\n";

    incr::examen_a::demo();
    std::cout << "\n";

    incr::examen_b::demo();
    std::cout << "\n";

    incr::extra::demo();

    return 0;
}
