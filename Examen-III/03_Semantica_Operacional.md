# Semántica Operacional — Revisión de Tipos e Interpretación

La semántica operacional formaliza el significado de los lenguajes de programación a través de juicios matemáticos. Describe tanto el análisis estático en tiempo de compilación (revisión de tipos) como la evaluación dinámica en tiempo de ejecución (interpretación).

---

## 1. Revisión de Tipos (Type Checking)

La revisión de tipos garantiza que un programa sea semánticamente correcto antes de ejecutarlo. Se modela mediante un sistema de inferencia donde un ambiente $\Gamma$ (contexto) mapea identificadores a sus tipos correspondientes. 

El juicio de tipado fundamental es $\Gamma \vdash e : T$, que se lee: "En el contexto $\Gamma$, la expresión $e$ tiene el tipo $T$". Mientras el *type checking* verifica que la expresión cumple con un tipo esperado explícito, la *type inference* (inferencia de tipos) deduce el tipo sin intervención del usuario a partir de sus operandos ([4-slides-ipl-book.md](/sources/4-slides-ipl-book.md)).

### 1.1 Contextos y Ámbitos Anidados

El ambiente $\Gamma$ no es una estructura plana. Para soportar el sombreado de variables (shadowing) en lenguajes estructurados en bloques, $\Gamma$ se implementa como una pila de tablas de símbolos.

Al entrar a un bloque `{`, se apila una nueva tabla. Las búsquedas de variables miran la cima de la pila primero y descienden hasta encontrar el identificador. Al salir del bloque `}`, se desapila la tabla, destruyendo las variables locales y restaurando el ámbito externo intacto. 

### 1.2 Implementación del Analizador

La implementación física del analizador semántico varía drásticamente según el paradigma de lenguaje en el que se construya.

```java
// Patrón Visitor en Java para recorrer el AST y tipar nodos
public class TypeChecker implements Visitor<Type, Environment> {
    public Type visit(AddExpr expr, Environment env) {
        Type left = expr.left.accept(this, env);
        Type right = expr.right.accept(this, env);
        if (left == Type.Int && right == Type.Int) return Type.Int;
        throw new TypeError("Suma requiere enteros");
    }
}
```

En orientación a objetos (Java/C++), el patrón Visitor permite despachar las reglas de inferencia a través del árbol de sintaxis abstracta (AST) sin modificar las clases de los nodos. En lenguajes funcionales puros como Haskell, el proceso se modela mediante una Mónada de Error o Estado, arrastrando el contexto puramente de función a función y abortando la cadena si la unificación falla.

---

## 2. Semántica de Paso Grande (Big-Step)

La semántica de paso grande o natural modela la relación directa entre una expresión y su valor final, ignorando los pasos intermedios de cómputo.

El juicio de evaluación es $\gamma \vdash e \Downarrow v$, significando "En el entorno dinámico $\gamma$, la expresión $e$ se evalúa al valor final $v$". Para las sentencias, el juicio cambia a $\gamma \vdash s \Downarrow \gamma'$, ya que una sentencia no retorna un valor, sino que muta el estado dinámico (la memoria) del programa, resultando en un nuevo entorno $\gamma'$.

### 2.1 Efectos Colaterales y Evaluación

Los efectos secundarios rompen la pureza funcional, haciendo que el orden de evaluación importe críticamente. En expresiones como `++x` (pre-incremento) el valor cambia antes de evaluarse en la expresión contenedora, mientras que `x++` (post-incremento) inyecta el valor actual en la expresión y muta el entorno después. 

Para operadores lógicos como `&&` y `||`, C/Java imponen semántica de evaluación perezosa estricta (short-circuiting). 

- Si se evalúa `A && B` y `A` resulta falso, `B` jamás se ejecuta. 
- Esto impide fallos catastróficos de memoria, como en el clásico: `if (ptr != null && ptr->value == 1)`.

### 2.2 Estrategias de Evaluación

- **Call-by-value:** Los argumentos se evalúan a sus valores finales *antes* de entrar a la función. Es predecible y estricto.
- **Call-by-name:** Los argumentos se pasan como expresiones no evaluadas (thunks) y se evalúan cada vez que son mencionados en el cuerpo de la función.
- **Call-by-need:** Idéntico a Call-by-name, pero su valor se memoiza tras la primera evaluación. Es la base de lenguajes puros y perezosos como Haskell.

---

## 3. Semántica de Paso Pequeño (Small-Step) y JVM

Mientras *big-step* modela el resultado, *small-step* describe minuciosamente la transición de estado de la máquina: $\langle Inst, Env \rangle \to \langle Env' \rangle$ ([5-slides-ipl-book.md](/sources/5-slides-ipl-book.md)).

### 3.1 Arquitectura de la JVM

La Máquina Virtual de Java es una máquina orientada a pilas, carente de registros de CPU físicos. Su estado computacional consiste en:

1. **Puntero de programa ($P$):** Indica la instrucción bytecode actual.
2. **Pila de operandos ($S$):** Almacena cálculos intermedios.
3. **Variables locales ($V$):** Arreglo indexado para variables locales.

Una suma simple `x = 2 + 3` no ocurre en registros, sino que empuja constantes (`bipush 2`, `bipush 3`), las reduce usando la operación destructiva `iadd`, y almacena el resultado en el arreglo local usando `istore`. Este modelo es drásticamente más fácil de portar y compilar.

### 3.2 Gestión de Memoria en Runtime: Mark-Sweep

La máquina virtual debe gestionar el Heap automáticamente. El algoritmo clásico de Garbage Collection es *Mark-Sweep*.

- **Fase de Marcaje (Mark):** Recorre todos los objetos alcanzables desde las raíces (variables en la pila o globales) marcando los nodos como "vivos".
- **Fase de Barrido (Sweep):** Recorre linealmente el heap entero y devuelve la memoria de cualquier bloque no marcado de vuelta al sistema.
