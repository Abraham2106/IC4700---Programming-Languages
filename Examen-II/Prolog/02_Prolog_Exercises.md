# Ejercicios de practica de Prolog

Estos ejercicios estan basados en los temas de `01_Prolog_Basics.md`. La idea es practicar tanto la parte **declarativa** como la **procedimental** de Prolog.

## Recomendaciones

- Intente resolver primero los ejercicios sin ejecutar Prolog.
- Cuando un ejercicio pida una sustitucion, escribala explicitamente.
- Cuando un ejercicio pida explicar el comportamiento, indique tanto **que significa** el programa como **como se ejecuta**.
- Si quiere validar sus respuestas, pruebe luego las consultas en SWI-Prolog.

## Parte 1. Sintaxis y estructura

### Ejercicio 1. Variables, constantes y atomos
Indique cuales de los siguientes elementos son:

- variable
- constante
- termino compuesto
- atomo bien formado
- expresion mal formada

Elementos:

1. `X` : variable 
2. `alice` : constante 
3. `parent(alice, bob)` : termino compuesto 
4. `parent(X)` : termino 
5. `f(g(alice), Y)`  :idk 
6. `mother(sarah, isaac)`   :termino compuesto 
7. `child(alice, bob, carol)` suponiendo que `child/2` mal formado 
8. `[H|T]` atomo 

### Ejercicio 2. Aridad
Suponga que en un programa aparecen los predicados `edge/2`, `male/1` y `append/3`.

Determine cuales de las siguientes expresiones respetan la aridad y cuales no:

1. `edge(alice, bob)`
2. `edge(alice)`
3. `male(lot)`
4. `male(lot, haran)`
5. `append([a], [b], [a,b])`
6. `append([a], [b])`

### Ejercicio 3. Terminos ground
Indique cuales de los siguientes terminos o atomos son **ground**:

1. `alice`
2. `f(X)`
3. `pair(alice, bob)`
4. `parent(X, bob)`
5. `append([], [a], [a])`
6. `[X, bob]`

## Parte 2. Unificacion

### Ejercicio 4. Unificacion basica
Para cada par de terminos, indique si unifican. Si unifican, escriba una unificadora mas general.

1. `f(X)` y `f(alice)`
2. `pair(X, bob)` y `pair(alice, Y)`
3. `g(X, X)` y `g(alice, bob)`
4. `parent(X, Y)` y `parent(alice, bob)`
5. `father(X, X)` y `father(alice, bob)`
6. `[H|T]` y `[alice, bob]`

### Ejercicio 5. Cuando falla la unificacion
Explique por que fallan las siguientes unificaciones:

1. `f(X)` con `g(X)`
2. `parent(alice)` con `parent(alice, bob)`
3. `pair(X, X)` con `pair(alice, bob)`
4. `mother(sarah, X)` con `father(sarah, isaac)`

### Ejercicio 6. Sustituciones
Considere la sustitucion:

```prolog
{ X -> alice, Y -> f(bob), Z -> [alice, bob] }
```

Aplique la sustitucion a:

1. `parent(X, Y)`
2. `g(Z, X)`
3. `pair(Y, Z)`

## Parte 3. Hechos, reglas y consultas

Considere el siguiente programa:

```prolog
father(haran, lot).
mother(sarah, isaac).
male(lot).
male(isaac).

parent(X, Y) :- father(X, Y).
parent(X, Y) :- mother(X, Y).

son(X, Y) :- parent(Y, X), male(X).
grandparent(X, Y) :- parent(X, Z), parent(Z, Y).
```

### Ejercicio 7. Lectura declarativa
Explique en lenguaje natural que significa cada una de estas reglas:

1. `parent(X, Y) :- father(X, Y).`
2. `parent(X, Y) :- mother(X, Y).`
3. `son(X, Y) :- parent(Y, X), male(X).`
4. `grandparent(X, Y) :- parent(X, Z), parent(Z, Y).`

### Ejercicio 8. Consultas verdaderas o falsas
Determine si las siguientes consultas deberian responder `true` o `false`:

1. `?- father(haran, lot).`
2. `?- parent(sarah, isaac).`
3. `?- son(lot, haran).`
4. `?- son(isaac, sarah).`
5. `?- grandparent(haran, isaac).`
6. `?- male(sarah).`

### Ejercicio 9. Consultas con variables
Indique las respuestas esperadas de Prolog para:

1. `?- parent(X, isaac).`
2. `?- son(X, haran).`
3. `?- son(isaac, Y).`
4. `?- parent(X, Y).`

### Ejercicio 10. Completitud
Explique por que la siguiente definicion puede ser correcta pero incompleta:

```prolog
son(X, Y) :- father(Y, X), male(X).
```

Luego proponga una mejora.

## Parte 4. Trazado de ejecucion

### Ejercicio 11. Resolvent
Use el programa de la parte 3.

Muestre las reducciones del resolvent para la consulta:

```prolog
?- son(lot, haran).
```

Debe indicar:

1. consulta inicial
2. regla aplicada
3. resolvent resultante
4. momento en que el resolvent queda vacio

### Ejercicio 12. Arbol de prueba
Dibuje un arbol de prueba para:

```prolog
?- son(isaac, sarah).
```

Puede usar texto plano.

### Ejercicio 13. Backtracking
Explique que ocurre proceduralmente con la consulta:

```prolog
?- parent(X, isaac).
```

Indique:

- que clausula de `parent/2` intenta primero Prolog
- si esa rama tiene exito o falla
- en que momento aparece el backtracking
- cual es la respuesta final

## Parte 5. Listas y recursion

Considere el programa:

```prolog
append([], Y, Y).
append([H|T], Y, [H|U]) :- append(T, Y, U).
```

### Ejercicio 14. Casos directos
Determine el resultado de las siguientes consultas:

1. `?- append([], [bob], Z).`
2. `?- append([alice], [bob], Z).`
3. `?- append([alice, bob], [carol], Z).`
4. `?- append([alice], Y, [alice, bob]).`
5. `?- append([alice], [bob], [alice, bob]).`

### Ejercicio 15. Traza de recursion
Muestre paso a paso como se resuelve:

```prolog
?- append([alice, bob], [carol], Z).
```

Sugerencia: vaya indicando como se instancian `H`, `T`, `U` y `Z`.

### Ejercicio 16. Consultas inversas
Explique que soluciones espera para:

```prolog
?- append(X, Y, [alice, bob]).
```

Indique todas las particiones posibles de la lista.

### Ejercicio 17. Definicion de `member/2`
Defina un predicado `member/2` para listas usando recursion.

Luego pruebe mentalmente estas consultas:

1. `?- member(alice, [alice, bob, carol]).`
2. `?- member(X, [alice, bob]).`
3. `?- member(carol, [alice, bob]).`

## Parte 6. Orden y terminacion

Considere estos dos programas.

Programa A:

```prolog
edge(alice, bob).

reach(X, X).
reach(X, Z) :- edge(X, Y), reach(Y, Z).
```

Programa B:

```prolog
edge(alice, bob).

reach(X, Z) :- reach(Y, Z), edge(X, Y).
reach(X, X).
```

### Ejercicio 18. Comparacion declarativa
Explique por que ambos programas parecen expresar una idea parecida desde el punto de vista logico.

### Ejercicio 19. Comparacion procedimental
Explique por que la consulta:

```prolog
?- reach(alice, Y).
```

se comporta mejor en el Programa A que en el Programa B.

### Ejercicio 20. Orden de metas
Considere estas dos reglas:

```prolog
p(X) :- q(X), r(X).
p(X) :- r(X), q(X).
```

Responda:

1. Desde un punto de vista puramente logico, que diferencia hay entre ellas?
2. Desde el punto de vista procedural de Prolog, por que podria importar el orden?

## Parte 7. Significado, correccion y completitud

### Ejercicio 21. Significado de un programa
Explique con sus palabras que significa `M(P)`, el significado de un programa logico.

### Ejercicio 22. Correccion
De un ejemplo de un programa pequeno que sea **incorrecto**, es decir, que deduzca algo que no deberia deducir.

### Ejercicio 23. Completitud
De un ejemplo de un programa pequeno que sea **incompleto**, es decir, que deje de deducir algo que intuitivamente deberia ser verdadero.

### Ejercicio 24. Analisis de especificacion
Considere esta base de conocimientos:

```prolog
father(haran, lot).
mother(sarah, isaac).
male(lot).
male(isaac).

son(X, Y) :- father(Y, X), male(X).
```

Con respecto al significado intencional de `son/2`, responda:

1. Es correcto?
2. Es completo?
3. Que hecho intuitivo falta poder deducir?

## Parte 8. Datalog

### Ejercicio 25. Restricciones
Indique cuales de las siguientes clausulas serian validas en Datalog y cuales no. Justifique.

1. `parent(alice, bob).`
2. `edge(X, Y) :- link(X, Y).`
3. `foo(X, Y) :- bar(X).`
4. `append(cons(alice, []), [], Z).`
5. `ancestor(X, Y) :- parent(X, Z), ancestor(Z, Y).`

### Ejercicio 26. Prolog vs Datalog
Explique dos diferencias importantes entre Prolog y Datalog.

### Ejercicio 27. Ventajas de Datalog
Por que Datalog ofrece mejores garantias de terminacion que Prolog?

## Parte 9. Programacion

Resuelva los siguientes ejercicios escribiendo reglas en Prolog.

### Ejercicio 28. `daughter/2`
Defina `daughter(X, Y)` que sea verdadero si `X` es hija de `Y`.

Puede asumir la existencia de:

```prolog
parent(X, Y).
female(X).
```

### Ejercicio 29. `grandchild/2`
Defina `grandchild(X, Y)` que sea verdadero si `X` es nieto o nieta de `Y`.

### Ejercicio 30. `sibling/2`
Defina `sibling(X, Y)` que sea verdadero si `X` y `Y` comparten al menos un progenitor y ademas `X \= Y`.

### Ejercicio 31. `ancestor/2`
Defina recursivamente `ancestor(X, Y)` para expresar que `X` es ancestro de `Y`.

Luego responda:

1. cual es el caso base?
2. cual es el caso recursivo?

### Ejercicio 32. Longitud de una lista
Defina `len(List, N)` para calcular la longitud de una lista.

Ejemplos esperados:

```prolog
?- len([], N).
N = 0.

?- len([a,b,c], N).
N = 3.
```

### Ejercicio 33. Ultimo elemento
Defina `last_elem(List, X)` que sea verdadero si `X` es el ultimo elemento de la lista.

### Ejercicio 34. Prefijo
Usando `append/3`, defina `prefix(P, L)` que sea verdadero si `P` es prefijo de `L`.

### Ejercicio 35. Sufijo
Usando `append/3`, defina `suffix(S, L)` que sea verdadero si `S` es sufijo de `L`.

## Parte 10. Preguntas de razonamiento

### Ejercicio 36. Declarativo vs procedural
Explique con un ejemplo por que entender solo la interpretacion declarativa no siempre basta para programar bien en Prolog.

### Ejercicio 37. Choice points
Que es un **choice point** y que relacion tiene con el backtracking?

### Ejercicio 38. Reduccion
Que significa decir que una meta se reduce usando una regla?

### Ejercicio 39. Consulta existencial
Por que una consulta con variables puede interpretarse como una pregunta existencial?

### Ejercicio 40. Diseno de programas
Mencione dos decisiones de diseno que pueden afectar fuertemente la terminacion o eficiencia de un programa Prolog.

## Reto integrador

### Ejercicio 41. Base de conocimiento familiar
Construya una base de conocimiento pequena sobre una familia de al menos 6 personas que incluya:

- hechos `father/2`
- hechos `mother/2`
- hechos `male/1`
- hechos `female/1`
- reglas `parent/2`, `son/2`, `daughter/2`, `grandparent/2`, `sibling/2`

Luego escriba 8 consultas de prueba:

- 4 ground
- 4 con variables

### Ejercicio 42. Mini reporte
Para la base de conocimiento del ejercicio anterior, escriba un analisis corto respondiendo:

1. Que relaciones puede deducir el programa?
2. Hay algo que sea correcto pero incompleto?
3. Que consultas podrian generar varias respuestas?
4. En que casos aparece backtracking?

## Sugerencia de estudio

Una buena forma de practicar es resolver los ejercicios en este orden:

1. Parte 1 y Parte 2
2. Parte 3 y Parte 4
3. Parte 5 y Parte 6
4. Parte 7 y Parte 8
5. Parte 9 y el reto integrador
