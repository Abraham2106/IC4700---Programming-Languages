# Guia de Prolog sobre listas y matrices

Esta guia extiende los apuntes basicos de Prolog con enfasis en estructuras de datos muy comunes:

- listas
- listas de listas
- matrices representadas como listas de filas

La idea es estudiar no solo **que hace** cada predicado, sino tambien **como se ejecuta** en Prolog.

## 1. Listas en Prolog

En Prolog, una lista puede escribirse asi:

```prolog
[]
[a]
[a, b, c]
```

Internamente, una lista se construye con la forma:

```prolog
[H|T]
```

donde:

- `H` es la cabeza
- `T` es la cola

Ejemplos:

- `[a, b, c]` equivale a `[a | [b, c]]`
- `[b, c]` equivale a `[b | [c]]`
- `[c]` equivale a `[c | []]`

Entonces:

```prolog
[a, b, c] = [a | [b, c]]
```

es verdadero.

## 2. Concatenacion de listas con `append/3`

Un predicado clasico es `append/3`, que concatena dos listas.

```prolog
append([], Y, Y).
append([H|T], Y, [H|U]) :- append(T, Y, U).
```

### Lectura declarativa

- La concatenacion de la lista vacia con `Y` es `Y`.
- Si la primera lista empieza con `H`, entonces el resultado tambien empieza con `H`.

### Lectura procedimental

Prolog va quitando la cabeza de la primera lista hasta llegar al caso base.
Luego reconstruye la respuesta al volver de la recursion.

### Ejemplos

```prolog
?- append([a, b], [c, d], Z).
Z = [a, b, c, d].
```

```prolog
?- append(X, Y, [a, b]).
X = [],
Y = [a, b] ;
X = [a],
Y = [b] ;
X = [a, b],
Y = [].
```

Esta segunda consulta muestra algo importante: en Prolog un predicado puede usarse en varios sentidos.

## 3. Inversa de una lista

### Version simple con `append/3`

Una forma elegante de definir la inversa es:

```prolog
reverse_list([], []).
reverse_list([H|T], R) :-
    reverse_list(T, RT),
    append(RT, [H], R).
```

### Idea

- La inversa de `[]` es `[]`.
- Para invertir `[H|T]`, primero se invierte `T`.
- Luego se agrega `H` al final.

### Ejemplo

```prolog
?- reverse_list([a, b, c], R).
R = [c, b, a].
```

### Explicacion paso a paso

Para:

```prolog
?- reverse_list([a, b, c], R).
```

Prolog hace conceptualmente esto:

1. invierte `[b, c]`
2. invierte `[c]`
3. invierte `[]`
4. obtiene `[]`
5. concatena `[]` con `[c]` y obtiene `[c]`
6. concatena `[c]` con `[b]` y obtiene `[c, b]`
7. concatena `[c, b]` con `[a]` y obtiene `[c, b, a]`

### Observacion importante

Esta version es facil de entender, pero no es la mas eficiente, porque `append/3` recorre listas repetidamente.

## 4. Inversa con acumulador

Una version mas eficiente usa un acumulador:

```prolog
reverse_acc(L, R) :- reverse_acc(L, [], R).

reverse_acc([], Acc, Acc).
reverse_acc([H|T], Acc, R) :-
    reverse_acc(T, [H|Acc], R).
```

### Idea

En vez de agregar al final, se va construyendo la respuesta al frente del acumulador.

### Ejemplo

```prolog
?- reverse_acc([a, b, c], R).
R = [c, b, a].
```

### Ventaja

Esta version suele ser mejor proceduralmente porque evita concatenaciones repetidas.

## 5. Longitud de una lista

```prolog
len([], 0).
len([_|T], N) :-
    len(T, M),
    N is M + 1.
```

### Explicacion

- La lista vacia tiene longitud `0`.
- Si la lista tiene una cabeza y una cola, su longitud es `1` mas la longitud de la cola.

### Ejemplos

```prolog
?- len([], N).
N = 0.
```

```prolog
?- len([a, b, c, d], N).
N = 4.
```

## 6. Suma de los elementos de una lista

Supongamos listas de numeros.

```prolog
sum_list([], 0).
sum_list([H|T], S) :-
    sum_list(T, ST),
    S is H + ST.
```

### Ejemplos

```prolog
?- sum_list([1, 2, 3, 4], S).
S = 10.
```

### Nota

Aqui aparece `is`, que evalua expresiones aritmeticas. No es solo unificacion.

## 7. Ultimo elemento de una lista

```prolog
last_elem([X], X).
last_elem([_|T], X) :-
    last_elem(T, X).
```

### Explicacion

- Si la lista tiene un solo elemento, ese es el ultimo.
- Si tiene mas de uno, se ignora la cabeza y se sigue con la cola.

### Ejemplo

```prolog
?- last_elem([a, b, c], X).
X = c.
```

## 8. Pertenencia

```prolog
member_list(X, [X|_]).
member_list(X, [_|T]) :-
    member_list(X, T).
```

### Ejemplos

```prolog
?- member_list(b, [a, b, c]).
true.
```

```prolog
?- member_list(X, [a, b]).
X = a ;
X = b.
```

## 9. Prefijos y sufijos

Podemos usar `append/3` para describir otras relaciones.

### Prefijo

```prolog
prefix(P, L) :-
    append(P, _, L).
```

### Sufijo

```prolog
suffix(S, L) :-
    append(_, S, L).
```

### Ejemplos

```prolog
?- prefix(P, [a, b, c]).
P = [] ;
P = [a] ;
P = [a, b] ;
P = [a, b, c].
```

```prolog
?- suffix(S, [a, b, c]).
S = [a, b, c] ;
S = [b, c] ;
S = [c] ;
S = [].
```

## 10. Matrices como listas de listas

En Prolog, una matriz puede representarse como una lista de filas:

```prolog
[
  [1, 2, 3],
  [4, 5, 6],
  [7, 8, 9]
]
```

Cada fila es una lista, y la matriz completa es una lista de filas.

## 11. Suma de una fila

Primero podemos reutilizar `sum_list/2`:

```prolog
sum_list([], 0).
sum_list([H|T], S) :-
    sum_list(T, ST),
    S is H + ST.
```

Entonces:

```prolog
?- sum_list([4, 5, 6], S).
S = 15.
```

## 12. Suma de todos los elementos de una matriz

```prolog
sum_matrix([], 0).
sum_matrix([Row|Rows], Total) :-
    sum_list(Row, RowSum),
    sum_matrix(Rows, RestSum),
    Total is RowSum + RestSum.
```

### Explicacion

- Si no hay filas, la suma total es `0`.
- Si hay una fila `Row`, se suma esa fila.
- Luego se suma el resto de filas.
- Finalmente se combinan ambos resultados.

### Ejemplo

```prolog
?- sum_matrix([[1,2],[3,4],[5,6]], S).
S = 21.
```

## 13. Recorrido de una matriz

Si queremos recorrer una matriz elemento por elemento, una estrategia natural es:

1. recorrer una fila
2. luego recorrer las filas restantes

Por ejemplo, para reconocer si un elemento aparece en alguna posicion:

```prolog
matrix_member(X, [Row|_]) :-
    member_list(X, Row).
matrix_member(X, [_|Rows]) :-
    matrix_member(X, Rows).
```

### Ejemplos

```prolog
?- matrix_member(5, [[1,2],[3,4],[5,6]]).
true.
```

```prolog
?- matrix_member(8, [[1,2],[3,4],[5,6]]).
false.
```

## 14. Aplanar una matriz

Aplanar significa convertir una lista de listas en una sola lista.

```prolog
flatten_matrix([], []).
flatten_matrix([Row|Rows], Flat) :-
    flatten_matrix(Rows, RestFlat),
    append(Row, RestFlat, Flat).
```

### Ejemplo

```prolog
?- flatten_matrix([[1,2],[3,4],[5]], R).
R = [1,2,3,4,5].
```

## 15. Transpuesta de una matriz

La transpuesta cambia filas por columnas.

Por ejemplo:

```text
[[1,2,3],
 [4,5,6]]
```

se convierte en:

```text
[[1,4],
 [2,5],
 [3,6]]
```

Una forma clasica de hacerlo en Prolog es:

```prolog
transpose([[]|_], []).
transpose(Matrix, [Row|Rows]) :-
    first_column(Matrix, Row, RestMatrix),
    transpose(RestMatrix, Rows).

first_column([], [], []).
first_column([[H|T]|Rows], [H|Hs], [T|Ts]) :-
    first_column(Rows, Hs, Ts).
```

### Idea

- `first_column/3` toma la primera columna de la matriz.
- Tambien devuelve la matriz restante sin esa columna.
- `transpose/2` repite el proceso hasta agotar las columnas.

### Ejemplo

```prolog
?- transpose([[1,2,3],[4,5,6]], T).
T = [[1,4],[2,5],[3,6]].
```

## 16. Suma por filas

Si queremos una lista con la suma de cada fila:

```prolog
row_sums([], []).
row_sums([Row|Rows], [S|Ss]) :-
    sum_list(Row, S),
    row_sums(Rows, Ss).
```

### Ejemplo

```prolog
?- row_sums([[1,2,3],[4,5],[6]], R).
R = [6, 9, 6].
```

## 17. Suma por columnas

Podemos reutilizar la transpuesta.

```prolog
column_sums(Matrix, Sums) :-
    transpose(Matrix, T),
    row_sums(T, Sums).
```

### Ejemplo

```prolog
?- column_sums([[1,2,3],[4,5,6]], R).
R = [5,7,9].
```

## 18. Observaciones procedimentales

Estas definiciones sirven para reforzar varias ideas importantes:

- muchas estructuras se procesan con recursion sobre `[H|T]`
- el caso base debe estar bien definido
- el orden de las metas puede afectar rendimiento y terminacion
- `append/3` es muy expresivo, pero a veces no es la opcion mas eficiente
- los acumuladores suelen mejorar el comportamiento procedural

Por ejemplo, `reverse_list/2` y `reverse_acc/2` expresan la misma idea declarativa, pero no tienen el mismo costo procedural.

## 19. Ejercicios propuestos

### Ejercicio 1. Concatenacion
Explique que responde Prolog en:

```prolog
?- append([a], [b, c], Z).
?- append(X, Y, [a, b]).
```

### Ejercicio 2. Inversa
Defina `reverse_list/2` y luego pruebela con:

```prolog
?- reverse_list([1,2,3,4], R).
```

### Ejercicio 3. Longitud
Defina `len/2` y evale:

```prolog
?- len([a,b,c,d,e], N).
```

### Ejercicio 4. Suma de lista
Defina `sum_list/2` y pruebe:

```prolog
?- sum_list([10,20,30], S).
```

### Ejercicio 5. Aplanar matriz
Defina `flatten_matrix/2` y pruebe:

```prolog
?- flatten_matrix([[1,2],[3],[4,5]], R).
```

### Ejercicio 6. Suma total de matriz
Defina `sum_matrix/2` y pruebe:

```prolog
?- sum_matrix([[1,2],[3,4],[5]], S).
```

### Ejercicio 7. Transpuesta
Defina `transpose/2` y pruebe:

```prolog
?- transpose([[1,2],[3,4],[5,6]], T).
```

### Ejercicio 8. Sumas por columna
Usando `transpose/2` y `row_sums/2`, calcule:

```prolog
?- column_sums([[1,2,3],[4,5,6],[7,8,9]], R).
```

### Ejercicio 9. Predicado propio
Defina `diagonal/2` para una matriz cuadrada `2x2` o `3x3`, que extraiga su diagonal principal.

### Ejercicio 10. Analisis
Compare:

- una version de `reverse` con `append/3`
- una version de `reverse` con acumulador

Explique cual es mejor proceduralmente y por que.

## 20. Resumen

Las listas en Prolog son una base excelente para aprender recursion, unificacion y procesamiento estructural. A partir de ellas se pueden construir facilmente predicados para:

- concatenar
- invertir
- contar
- sumar
- buscar
- recorrer matrices
- transponer matrices
- sumar filas y columnas

El punto clave es siempre el mismo:

- identificar el caso base
- descomponer la estructura con `[H|T]`
- definir claramente la recursion

---

## 21. Soluciones Destacadas para Listas y Matrices

### Ejercicio 9. Extraer Diagonal Principal
Para extraer la diagonal de una matriz cuadrada de cualquier tamaño:
```prolog
diagonal(Matrix, Diagonal) :-
    diagonal(Matrix, 0, Diagonal).

% Caso base: matriz vacia
diagonal([], _, []).
% Caso recursivo: extrae el elemento K-esimo de la primera fila, e incrementa K
diagonal([Row|Rows], K, [X|Rest]) :-
    nth0(K, Row, X),
    K1 is K + 1,
    diagonal(Rows, K1, Rest).
```

### Ejercicio 10. Análisis de Complejidad: `reverse_list/2` vs `reverse_acc/3`
- **Versión con `append/3` ($O(N^2)$)**:
  - En cada llamada recursiva de `reverse_list([H|T], R)`, Prolog primero invierte la cola (costo recurrente) y luego ejecuta `append(RT, [H], R)`.
  - Como `append/3` sobre una lista de tamaño $K$ toma tiempo $O(K)$, la relación de recurrencia es $T(N) = T(N-1) + O(N)$, lo que resulta en un tiempo total de **$O(N^2)$**.
- **Versión con acumulador ($O(N)$)**:
  - En `reverse_acc([H|T], Acc, R)`, el elemento `H` se coloca al frente del acumulador usando la unificación `[H|Acc]`, la cual ocurre en tiempo constante **$O(1)$**.
  - La relación de recurrencia es $T(N) = T(N-1) + O(1)$, lo que resulta en un tiempo total de **$O(N)$** y es además optimizable mediante **recursión de cola** (Tail Call Optimization - TCO), lo que significa que consume $O(1)$ espacio en la pila de llamadas.
