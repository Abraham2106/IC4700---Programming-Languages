# Guia de Recursion de Cola en Prolog

Esta guia explica como transformar un predicado recursivo comun en uno con recursion de cola usando acumuladores.

Temas cubiertos:

- que es recursion de cola
- por que importa
- inversion de listas con acumulador
- traza mental de ejecucion

## 1. Que es recursion de cola

Una llamada recursiva es de cola si es la ultima operacion que hace la funcion o predicado antes de terminar.

Eso significa:

- no queda trabajo pendiente despues de la llamada recursiva

En esos casos, muchos sistemas pueden reutilizar el frame actual y evitar crecimiento innecesario de pila.

La intuicion operacional es:

- si no queda nada por hacer despues de volver de la recursion
- entonces no hace falta "guardar tanto contexto"

Eso es justamente lo que vuelve atractiva la recursion de cola para procesos largos o listas grandes.

## 2. Idea general en Prolog

En Prolog, la recursion de cola suele lograrse con un acumulador.

En vez de:

- hacer recursion
- volver
- y luego reconstruir la respuesta

se va construyendo la respuesta durante la recursion.

En Prolog esto es especialmente comun porque muchos predicados sobre listas siguen un patron de recorrido lineal:

- leer cabeza
- actualizar algun estado parcial
- seguir con la cola

Ese "estado parcial" suele convertirse en el acumulador.

## 3. Version no optimizada de invertir

Una version tipica pero no de cola seria:

```prolog
invierte([], []).
invierte([H|T], R) :-
    invierte(T, RT),
    append(RT, [H], R).
```

### Problema

Despues de la llamada recursiva:

```prolog
invierte(T, RT)
```

todavia queda trabajo pendiente:

```prolog
append(RT, [H], R)
```

Por eso no es recursion de cola.

## 4. Version con acumulador

La forma pedida es esta:

```prolog
invierte(L, R) :-
    invierte(L, [], R).

invierte([], Acc, Acc).
invierte([H|T], Acc, R) :-
    invierte(T, [H|Acc], R).
```

## 5. Explicacion

La idea es:

- `Acc` va guardando la lista invertida parcial
- cada nuevo elemento `H` se pone al frente del acumulador

Entonces:

- al llegar al caso base
- el acumulador ya contiene la respuesta final

Esto evita una operacion costosa muy comun en versiones no optimizadas: reconstruir la lista al regreso de la recursion.

En la version no de cola, cada nivel espera el resultado de abajo para hacer algo mas.
En la version de cola, cada nivel deja el trabajo ya adelantado antes de bajar.

## 6. Consulta esperada

```prolog
?- invierte([1,2,3,4], X).
X = [4,3,2,1].
```

Tambien puede verse la llamada interna:

```prolog
?- invierte([1,2,3,4], [], X).
X = [4,3,2,1].
```

## 7. Traza paso a paso

Inicio:

```text
invierte([1,2,3,4], [], X)
```

Primer paso:

```text
invierte([2,3,4], [1], X)
```

Segundo paso:

```text
invierte([3,4], [2,1], X)
```

Tercer paso:

```text
invierte([4], [3,2,1], X)
```

Cuarto paso:

```text
invierte([], [4,3,2,1], X)
```

Caso base:

```text
X = [4,3,2,1]
```

## 8. Por que esto si es recursion de cola

En la regla:

```prolog
invierte([H|T], Acc, R) :-
    invierte(T, [H|Acc], R).
```

la llamada recursiva es lo ultimo que ocurre.

No queda ninguna meta despues.

Ese detalle es exactamente el criterio importante.

Compare:

```prolog
p(X, R) :-
    q(X, Y),
    r(Y, R).
```

Si la llamada recursiva estuviera en `q/2`, no seria de cola, porque despues queda `r(Y, R)`.

En cambio:

```prolog
p(X, R) :-
    q(X, R).
```

si `q/2` es recursiva, entonces la recursion esta en posicion de cola.

## 9. Patron general

Muchos predicados sobre listas pueden transformarse con este esquema:

```prolog
predicado(L, R) :-
    predicado(L, AccInicial, R).

predicado([], Acc, Acc).
predicado([H|T], Acc, R) :-
    NuevoAcc = ...,
    predicado(T, NuevoAcc, R).
```

## 10. Cuando conviene

Conviene especialmente cuando:

- la lista puede ser grande
- se quiere evitar trabajo pendiente al volver
- se busca una version mas eficiente proceduralmente

Tambien conviene cuando el predicado esta pensado como un "loop logico" que debe mantenerse por mucho tiempo, como pasa conceptualmente en servidores escritos con recursion de cola en Erlang.

## 11. Comparacion conceptual

Version no de cola:

- descompone
- llama recursivamente
- reconstruye al volver

Version de cola:

- descompone
- actualiza acumulador
- sigue

## 12. Ideas clave para estudiar

- recursion de cola significa que no queda trabajo pendiente
- en Prolog, eso suele lograrse con acumuladores
- invertir lista con acumulador es un ejemplo clasico
- `invierte([], Acc, Acc)` expresa que el acumulador ya es la respuesta

## 13. Comparacion de costo intuitivo

Sin entrar en una prueba formal de complejidad, la intuicion es:

- la version con `append/3` vuelve a recorrer listas parciales muchas veces
- la version con acumulador hace solo inserciones al frente

Por eso la segunda suele comportarse mejor en la practica.
