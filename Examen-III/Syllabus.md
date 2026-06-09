# Syllabus del Examen III — Temario Estructurado y Guía de Contenidos

Este documento establece el temario oficial del Examen III, integrando los conceptos fundamentales de sistemas de tipos, semántica de lenguajes, el modelo operacional de ejecución, y las particularidades de diseño de C++ y Prolog.

---

## 1. Temario Oficial

### 1.1 Principio de Sustitución de Liskov (LSP)
El LSP es la regla fundamental que rige el diseño de jerarquías de tipos seguras en la programación orientada a objetos.
- **Teoría Formal:** Subtipado y substitución estricta. Relación con las invariantes de la clase base.
- **Co-varianza:** Flexibilización de los tipos de retorno en métodos sobreescritos de subclases (soportado en C++ para punteros y referencias).
- **Contra-varianza:** Flexibilización de los tipos de argumentos en firmas de métodos en subclases (no soportado en C++ para métodos virtuales, donde se interpreta como sobrecarga).
- **Violación de LSP en C++:** Cómo la herencia privada (`private inheritance`) rompe la relación "es-un" al ocultar la interfaz base, provocando que los casts a clases base privadas sean ilegales.

### 1.2 Idiosincrasias del C++
Particularidades de C++ en la gestión de memoria física, control de acceso y ciclo de vida de objetos.
- **Uso de `const` y Const-Correctness:** Variables inmutables, punteros a constantes (`const T*`) vs. punteros constantes (`T* const`), parámetros constantes (por valor, referencia y puntero) y métodos constantes (donde `this` se tipa como `const T* const`).
- **Pila (Stack) vs. Heap:** Ciclo de vida determinista de objetos en la pila frente a la gestión manual en el heap (`new` y `delete`). El peligro de referencias colgantes, fugas de memoria y doble liberación.
- **Ciclo de Vida del Objeto:**
  - **Constructores:** Constructor por defecto, con argumentos, constructor por copia (`X(const X&)`), constructores protegidos/privados para evitar instanciación, e instancias anónimas.
  - **Listas de Inicialización:** Inicialización directa de miembros vs. asignación en el cuerpo del constructor, y el orden físico de construcción (clases base -> variables miembro en orden de declaración -> cuerpo del constructor).
  - **Destructores:** Orden de destrucción inverso a la construcción, destructores globales y la importancia crítica de los destructores virtuales en polimorfismo dinámico. Invocación explícita del destructor.
- **Ámbitos, Visibilidad y Enlazado:** Clases, estructuras (miembros públicos por defecto) y uniones (uniones anónimas y restricciones de miembros). Ámbito global, colisiones de enlazado y namespaces.
- **Sobrecarga de Operadores y Conversiones:**
  - Operadores miembro vs. no miembro (uso de funciones `friend`).
  - Sobrecarga especial: operador de asignación (`operator=`) y la Regla de Tres, operadores de acceso (`->`), índice (`[]`), y llamada a función (`()`).
  - Sobrecarga de alocación: `operator new` y `operator delete` personalizados, y placement new.
  - Operadores de conversión implícita y explícita de tipos.
- **Inicialización Global:** Problemas de orden de inicialización de objetos globales entre unidades de traducción y el patrón de inicialización con variables dummy (iostreams).

### 1.3 Semántica Operacional
El modelo formal para validar, tipar e interpretar programas de software.
- **Revisión de Tipos (Type Checking):**
  - Juicios de tipado ($\Gamma \vdash e : T$) y reglas de inferencia.
  - Type checking (validar un tipo dado) vs. Type inference (deducir el tipo de una expresión).
  - Contextos y Ambientes ($\Gamma$) representados como pilas de tablas de símbolos para gestionar ámbitos anidados y bloques.
  - Tratamiento de la sobrecarga y conversiones implícitas de tipos.
  - Implementación: Estructuración en Haskell usando mónadas de error e implementación en Java mediante el patrón de diseño Visitor.
- **Interpretación (Interpreters):**
  - **Semántica de Paso Grande (Big-Step Semantics):** Modelado del comportamiento de evaluación de expresiones ($\gamma \vdash e \Downarrow v$) y ejecución de sentencias ($\gamma \vdash s \Downarrow \gamma'$).
  - **Efectos Colaterales:** Modificación de ambientes ($\gamma(x := v)$), pre-incremento (`++x`) y post-incremento (`x++`), y evaluación perezosa en operadores lógicos (`&&` y `||`).
  - **Estrategias de Evaluación:** Diferencias entre Call-by-value, Call-by-name (evaluación perezosa teórica) y Call-by-need (evaluación perezosa optimizada).
  - **Semántica de Paso Pequeño (Small-Step Semantics) y JVM:**
    - Modelo de transición de estados: $\langle Instruction, Env \rangle \to \langle Env' \rangle$.
    - Estructura de la JVM: Puntero de programa $P$, Pila de operandos $S$ y almacenamiento de variables locales $V$.
    - Instrucciones bytecode fundamentales: `bipush`, `iadd`, `imul`, `iload`, `istore`, `ifeq`, `goto`.
  - **Gestión de Memoria en Runtime:** Representación física del Heap, recolección de basura automática y el funcionamiento del algoritmo Mark-Sweep (roots, mark y sweep).

### 1.4 Prolog (seguimiento)
Revisión y profundización en el paradigma de programación lógica.
- **Fundamentos del Paradigma:** Hechos, reglas, unificación y motor de inferencia (backtracking y orden de resolución).
- **Procesamiento de Listas:** Manipulación por cabeza y cola (`[H|T]`), recursión y predicados de biblioteca.
- **CSP (Constraint Satisfaction Problems) y Datalog:** Representación relacional de datos y solución de problemas declarativos con restricciones.

---

## 2. Fuentes de Referencia

Las notas de estudio de este temario deben extraerse y consolidarse a partir de las siguientes fuentes físicas:
1. `sources/4-slides-ipl-book.md`: Diapositivas del capítulo de revisión de tipos (Aarne Ranta).
2. `sources/5-slides-ipl-book.md`: Diapositivas del capítulo de intérpretes y semántica operacional (Aarne Ranta).
3. `sources/idiosincracias_cpp(1).md`: Notas detalladas sobre las particularidades y esquinas del lenguaje C++ (José Castro).
4. Carpeta `C++`: Guías de estudio locales (archivos del `01` al `34`).
5. Carpeta `Examen-II/Prolog`: Guías de estudio de fundamentos y listas de Prolog (archivos del `01` al `04`).
