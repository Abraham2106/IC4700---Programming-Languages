# Prolog — Estructuras Recursivas y Satisfacción de Restricciones (CSP)

El procesamiento de estructuras compuestas en Prolog se realiza destructivamente a nivel lógico a través de unificación estructurada. La aplicación práctica más prominente de esta desestructuración recursiva es la resolución de Problemas de Satisfacción de Restricciones (CSP), donde Prolog demuestra su poder como motor de búsqueda.

---

## 1. Listas y Recursión Estructural

Prolog no posee bucles imperativos `for` ni estructuras mutables nativas. El procesamiento de colecciones se hace a través del emparejamiento de patrones de la notación de cabeza y cola (`[H|T]`).

### 1.1 El Motor de Concatenación (`append/3`)

La función `append/3` revela la naturaleza multidireccional de Prolog.
```prolog
append([], Y, Y).
append([H|T], Y, [H|U]) :- append(T, Y, U).
```
Debido al motor relacional, esta sola definición permite:
1. Concatenar dos listas: `append([a], [b], Z)` (retorna `Z = [a,b]`).
2. Restar listas: `append([a], Y, [a,b])` (retorna `Y = [b]`).
3. Generar todas las particiones posibles de una lista: `append(X, Y, [a,b])` (retorna `([], [a,b])`, `([a], [b])`, `([a,b], [])`).

### 1.2 Costo Procedimental y Acumuladores

El paradigma declarativo esconde costos asimétricos ocultos. 

**Inversa Ingenua ($O(N^2)$):**
```prolog
reverse_list([], []).
reverse_list([H|T], R) :- reverse_list(T, RT), append(RT, [H], R).
```
Esta implementación debe recorrer recursivamente la cola $T$ hasta el fondo, retornar el arreglo invertido, y luego invocar `append` (que tiene un costo lineal sobre su primer argumento). Esta anidación produce un costo cuadrático de desestructuración.

**Inversa con Acumulador ($O(N)$ y Tail Recursion):**
```prolog
reverse_acc([], Acc, Acc).
reverse_acc([H|T], Acc, R) :- reverse_acc(T, [H|Acc], R).
```
Al inyectar el elemento `H` dinámicamente frente al acumulador mediante una simple unificación en el encabezado de llamada `[H|Acc]`, el costo se reduce a tiempo constante $O(1)$ por llamada recursiva, resultando en un costo total lineal. 
Además, como la llamada recursiva es estrictamente la **última** instrucción a evaluar (Tail Recursion), Prolog descarta el marco de llamada actual (Stack Frame), convirtiendo el proceso recursivo en un bucle iterativo de hardware puro (Optimización TCO).

---

## 2. Matrices y Recorridos Aplanados

Una matriz se representa naturalmente como una lista de listas (`[[1,2], [3,4]]`).

Operaciones clásicas como el aplastamiento (Flatten) requieren iteración destructiva en dos dimensiones:
```prolog
flatten_matrix([], []).
flatten_matrix([Row|Rows], Flat) :-
    flatten_matrix(Rows, RestFlat),
    append(Row, RestFlat, Flat).
```

Y extraer una diagonal exige mantener arrastre de índice aritmético (`K1 is K + 1`) usando predicados de evaluación nativa explícita (el operador `is` invoca a la ALU, no al unificador lógico).

---

## 3. Problemas de Satisfacción de Restricciones (CSP)

Un CSP formalmente es un conjunto de variables acotadas por dominios finitos, interconectadas por restricciones lógicas.

Resolver un mapa de grafos coloreados en Prolog (vecinos no pueden tener el mismo color) revela inmediatamente la diferencia abismal de rendimiento según el orden de evaluación del motor DFS subyacente.

### 3.1 El Anti-Patrón: Generate-and-Test (Generar y Probar)

Una codificación ingenua instanciaría el espacio probabilístico masivo de las variables y luego aplicaría los filtros de validez:
```prolog
colorear(A,B,C) :-
  color(A), color(B), color(C),
  A \= B, A \= C, B \= C.
```
Prolog desciende por las profundidades del árbol de decisión (DFS) asignando todos los colores. Solo cuando llega al final evalúa `A \= B`. Si la asignación original fue defectuosa, habrá desperdiciado tiempo evaluando inútilmente iteraciones sobre `C` antes de aplicar el *Backtracking*. Esta complejidad es pura fuerza bruta exponencial.

### 3.2 Optimización: Constrain-and-Generate (Comprobación Anticipada)

El entrelazado de restricciones con la asignación probabilística destruye el espacio de permutación tempranamente:
```prolog
colorear(A,B,C) :-
  color(A), 
  color(B), A \= B,  % Si B colisiona, se aborta y se hace backtracking inmediatamente, ignorando C.
  color(C), A \= C, B \= C.
```
Aquí la poda de ramas (Branch Pruning) es instantánea. Prolog aplica retroceso cronológico sobre $B$ sin jamás descender estúpidamente al nivel de la variable $C$.

Sistemas más avanzados como CLP(FD) (Constraint Logic Programming over Finite Domains) evitan este "backtracking activo" y aplican propagación de restricciones matemáticas directas sobre los dominios, permitiendo restricciones algebraicas sin enumerar el espacio de estados.
