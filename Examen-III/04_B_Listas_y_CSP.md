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

---

## 4. Recorridos de Árboles y Listas por Diferencia (Examen P1)

En Prolog los árboles binarios se representan con dos functores: `nodo(Valor, Izq, Der)` para nodos internos y `nil` para el árbol vacío. El objetivo típico es convertir un árbol en una lista según el orden de recorrido.

### 4.1 Preorder con `append` (raíz → izquierda → derecha)

La versión directa usa `append` para concatenar sublistas. Es correcta pero costosa ($O(n^2)$ por las concatenaciones sucesivas):

```prolog
% tree2list(+Arbol, -Lista)
tree2list(nil, []) :- !.
tree2list(nodo(X, Izq, Der), [X | Hijos]) :-
    tree2list(Izq, List_izq),
    tree2list(Der, List_der),
    append(List_izq, List_der, Hijos).
```

### 4.2 In-order con `append` (izquierda → raíz → derecha)

La raíz `X` se coloca *entre* la lista izquierda y la derecha. Se usa `[X | List_der]` como segundo argumento de `append` para intercalar la raíz:

```prolog
% tree2inorder(+Arbol, -Lista)
tree2inorder(nil, []) :- !.
tree2inorder(nodo(X, Izq, Der), Lista) :-
    tree2inorder(Izq, List_izq),
    tree2inorder(Der, List_der),
    append(List_izq, [X | List_der], Lista).
```

**Idea clave:** `Lista = List_izq ++ [X] ++ List_der`. Al escribir `[X | List_der]` como segundo argumento de `append`, la raíz queda justo antes del recorrido derecho.

### 4.3 Preorder con recursión de cola — Listas por Diferencia (sin `append`)

Las **listas por diferencia** representan una lista incompleta como un par `(Lista, Cola)`, donde `Cola` es la variable libre que marca el "hueco" al final. Dos listas por diferencia se concatenan en $O(1)$ unificando el hueco de la primera con el inicio de la segunda.

```prolog
% Interfaz pública: convierte el árbol en su preorder
tree2preorder(Arbol, Lista) :-
    preorder_dl(Arbol, Lista, []).

% preorder_dl(+Arbol, ?Lista, ?Cola)
% Invariante: Lista = preorder(Arbol) seguido de Cola.
preorder_dl(nil, Cola, Cola) :- !.
preorder_dl(nodo(X, Izq, Der), [X | Resto], Cola) :-
    preorder_dl(Izq, Resto, RestoDer),
    preorder_dl(Der, RestoDer, Cola).
```

**¿Por qué evita `append`?**

| Paso | Qué sucede |
|---|---|
| `[X \| Resto]` en la cabeza | La raíz se coloca directamente; no hay concatenación. |
| `preorder_dl(Izq, Resto, RestoDer)` | El subárbol izquierdo "llena" `Resto` dejando `RestoDer` como su cola. |
| `preorder_dl(Der, RestoDer, Cola)` | El derecho empieza justo donde terminó el izquierdo. |
| Caso base `nil` | Unifica `Cola` consigo misma — el "hueco" queda como está. |

Las listas se "cosen" con variables compartidas. La última llamada queda en **posición de cola** (tail position), habilitando TCO.

### 4.4 Traza de ejemplo

Para el árbol `nodo(1, nodo(2, nil, nil), nodo(3, nil, nil))`:

```
preorder_dl(nodo(1,L2,L3), Lista, [])
  → Lista = [1 | Resto]
  preorder_dl(nodo(2,nil,nil), Resto, RestoDer)
    → Resto = [2 | R2]
    preorder_dl(nil, R2, RestoDer)  → R2 = RestoDer
  preorder_dl(nodo(3,nil,nil), RestoDer, [])
    → RestoDer = [3 | R3]
    preorder_dl(nil, R3, [])  → R3 = []

Resultado: Lista = [1, 2, 3]   (sin llamar a append ni una sola vez)
```

