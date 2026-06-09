# Semántica Operacional — Interpretación y Máquinas Virtuales

La interpretación es el puente entre la sintaxis estática y el hardware dinámico. Ejecuta programas evaluando sus efectos colaterales paso a paso, ya sea interpretando directamente el Árbol de Sintaxis Abstracta (AST) de alto nivel, o procesando código de bytes en una máquina virtual de bajo nivel.

---

## 1. Semántica Operacional (Big-Step Semantics)

La ejecución pura del código fuente (Intérprete AST) se formaliza mediante la Semántica Operacional. A diferencia del chequeo de tipos, las reglas de evaluación devuelven *valores físicos* y modifican el estado de la memoria del programa.

### 1.1 Evaluaciones y Efectos Colaterales (Side Effects)

La forma de juicio para evaluar una expresión incorpora el concepto crítico de estado mutado:
$$\gamma \vdash e \Downarrow \langle v, \gamma' \rangle$$
*(Se lee: "En el entorno $\gamma$, la expresión $e$ se evalúa hacia el valor numérico $v$, mutando la memoria hacia un nuevo estado $\gamma'$").*

La mutación, denotada como $\gamma(x:=v)$, indica que el valor de $x$ fue sobrescrito, aniquilando su estado previo.

### 1.2 Semántica de Incrementos e Instrucciones Lógicas

La Semántica Operacional delata los detalles profundos de operadores como los incrementos:
- **Pre-incremento (`++x`):** Retorna $v+1$ y almacena $v+1$ en el nuevo entorno $\gamma'$.
- **Post-incremento (`x++`):** Retorna el antiguo valor $v$, pero el entorno $\gamma'$ almacena $v+1$. Esto provoca una "contaminación" de efectos si se evalúa una secuencia como `x++ - ++x`.

Para operaciones lógicas (`&&`, `||`), los intérpretes aplican **Lazy Evaluation** (Cortocircuito). La regla para `&&` evalúa la rama izquierda primero; si es 0 (falsa), aborta inmediatamente devolviendo 0 sin siquiera tocar el AST de la rama derecha.

### 1.3 Control de Flujo de Sentencias

A diferencia de las expresiones, las sentencias (Statements) se ejecutan puramente por sus efectos colaterales (modificar variables en $\gamma$), no devuelven valores: $\gamma \vdash s \Downarrow \gamma'$.
- En un bucle `while`, el entorno de salida $\gamma'$ de la condición se alimenta como entorno de entrada al cuerpo del bucle, y el entorno resultante se realimenta recursivamente.

---

## 2. Estrategias de Evaluación de Argumentos

El modo en que los lenguajes pasan argumentos a funciones define el rendimiento físico del hardware.

- **Call by Value (Paso por valor - C/Java):** Todos los argumentos de entrada en la invocación de función `f(a,b)` se calculan y evalúan por completo antes de saltar al bloque del cuerpo de la función. El entorno del cuerpo recibe datos puros (literales).
- **Call by Name (Paso por nombre - Macros C):** El código fuente en bruto del argumento se inyecta sintácticamente en cada aparición dentro de la función. Si la función nunca usa la variable, nunca se evalúa (Evaluation Lazy). Si la usa 100 veces, se recalcula ineficientemente 100 veces.
- **Call by Need (Haskell):** Similar a Call by Name, pero memoriza el resultado. Si la usa 100 veces, solo evalúa en la primera llamada y cachea el resultado.

---

## 3. Máquinas Virtuales (JVM) y Semántica Small-Step

Ejecutar lenguajes empresariales interpretando el AST puro es increíblemente lento. Lenguajes como Java optan por compilar a un modelo de pila virtual simplificado (Java Bytecode). 

### 3.1 Transiciones Small-Step

En lugar de reglas masivas para un árbol completo (Big-Step), el intérprete JVM utiliza transiciones pequeñas (Small-Step Semantics) basadas en un contador de programa (P), un almacenamiento de variables (V) y una Pila operacional (S):
$$\langle \text{Instruction}, P, V, S \rangle \rightarrow \langle P', V', S' \rangle$$

**Ejemplo - Despacho de Bytecode:**
- `iload 0`: Empuja la variable en el índice 0 del entorno $V$ hacia la cima de la Pila $S$. Modifica $P \rightarrow P+1$.
- `iadd`: Remueve los 2 valores en la cima de $S$, calcula la suma aritmética pura, y empuja el resultado lógico de vuelta a la cima de $S$.

### 3.2 Control de Saltos Físicos

Los bucles `while` y condicionales `if` no existen en este bajo nivel. El bytecode los pulveriza utilizando saltos explícitos (GOTO) y comprobadores de bandera (`ifeq` para "salta si el tope de la pila es 0").

---

## 4. Gestión de Memoria Dinámica (Garbage Collection)

Las variables en el stack ($V$) de la JVM operan sobre primitivos de 32/64 bits (enteros, booleanos). Cuando se instancia una clase compleja o un arreglo masivo (como `String`), el tamaño es arbitrario. 

La variable en $V$ albergará estrictamente un puntero primitivo de tamaño fijo que apunta a un bloque crudo en el Heap. A medida que las funciones retornan, las direcciones del Stack mueren, pero los inmensos bloques dinámicos del Heap quedan flotando y devorando la RAM.

### 4.1 Algoritmo Mark-Sweep

Para solventar las fugas masivas, los intérpretes implementan la recolección automática de basura (GC). El enfoque fundacional "Mark-Sweep" detiene la máquina virtual y opera en tres fases:
1. **Roots:** Escanea todo el Stack ($V$) extrayendo cualquier puntero primitivo vivo.
2. **Mark:** A partir de esas raíces vivas, recorre el grafo de los objetos referenciados en el Heap, marcando una bandera booleana (`true`) en su cabecera.
3. **Sweep:** El intérprete barre la memoria física bit a bit desde la posición 0. Cualquier bloque en el Heap que no tenga la marca de vida `true` es declarado basura y sobrescrito inmediatamente por estructuras "Unused" para su reutilización, desfragmentando efectivamente el sistema.
