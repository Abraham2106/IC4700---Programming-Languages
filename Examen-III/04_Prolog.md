# Prolog — Programación Lógica y Restricciones

El paradigma lógico invierte el modelo tradicional de programación: en lugar de describir los pasos para resolver un problema, se describen las verdades del universo del problema, delegando la resolución al motor de inferencia.

---

## 1. Fundamentos del Paradigma

Prolog no posee flujo de control explícito tradicional (bucles lógicos iterativos, if-else, etc.). Opera mediante una base de datos de conocimiento compuesto por axiomas lógicos: hechos (verdades incondicionales) y reglas (verdades condicionales).

### 1.1 Unificación y Resolución

El núcleo de Prolog es el algoritmo de unificación, una forma de igualación de patrones bidireccional donde el motor intenta instanciar variables libres para hacer que dos términos lógicos coincidan estructuralmente.

Cuando se emite una consulta, el motor busca resolverla probando los axiomas de la base de conocimiento de arriba hacia abajo (orden de cláusulas) y resolviendo las submetas de izquierda a derecha. Si el motor llega a un callejón sin salida (falla), aplica backtracking (marcha atrás): deshace las últimas unificaciones y prueba la siguiente rama del árbol de búsqueda lógico ([01_Prolog_Basics.md](/Examen-II/Prolog/01_Prolog_Basics.md)).

---

## 2. Procesamiento de Listas

Las listas en Prolog no son arreglos iterables, sino estructuras de datos recursivas. Consisten explícitamente en una cabeza (el primer elemento) y una cola (el resto de la lista).

### 2.1 El Patrón Cabeza/Cola [H|T]

La manipulación de listas en Prolog requiere pensamiento recursivo. El patrón de diseño más común es tener un caso base (lista vacía `[]` o lista de 1 elemento) y un caso recursivo que separa `[H|T]` para operar sobre la cabeza y pasar la cola como una nueva submeta.

```prolog
% Caso base: La suma de una lista vacía es 0
sumar_lista([], 0).

% Caso recursivo: Separamos la lista, sumamos la cola y añadimos la cabeza
sumar_lista([H|T], Suma) :-
    sumar_lista(T, SumaCola),
    Suma is H + SumaCola.
```

El operador `is` fuerza la evaluación aritmética del lado derecho; un simple `=` en su lugar solo intentaría la unificación estructural simbólica de los operandos ([03_Prolog_Lists.md](/Examen-II/Prolog/03_Prolog_Lists.md)).

---

## 3. CSP y Datalog

Prolog brilla en problemas donde el espacio de búsqueda es finito pero complejo.

### 3.1 Problemas de Satisfacción de Restricciones (CSP)

El modelado CSP (Constraint Satisfaction Problems) utiliza el patrón de "Generate-and-Test". Primero se genera un estado candidato y luego se somete a un filtro de validación o restricción. Esto lo hace ideal para problemas de lógica, horarios y coloreado de grafos.

Para optimizar, Prolog soporta librerías puras de restricciones lógicas sobre dominios finitos (CLP(FD)) donde el motor de búsqueda descarta ramas muertas del árbol agresivamente antes de instanciar todas las variables, mejorando el rendimiento enormemente ([04_Prolog_CSP_and_Datalog.md](/Examen-II/Prolog/04_Prolog_CSP_and_Datalog.md)).

### 3.2 Datalog

Datalog es un subconjunto restringido de Prolog utilizado primariamente para consultas de bases de datos lógicas relacionales. Carece de listas recursivas complejas, functores n-arios dinámicos o la construcción `! ` (corte). 

Esta limitación estructural tiene una gran recompensa teórica: Datalog garantiza terminación y es decidible (no caerá en bucles infinitos como puede hacerlo el motor general de Prolog al buscar recursión por la izquierda extrema). Datalog usa evaluación bottom-up (de las hojas hacia la meta), en contraposición a la evaluación top-down tradicional de Prolog.
