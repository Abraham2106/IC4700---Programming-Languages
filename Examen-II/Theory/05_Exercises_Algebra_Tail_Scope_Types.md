# Ejercicios de Tipos, Recursion de Cola y Ambito

Esta practica reune ejercicios cortos sobre los temas de la carpeta.

## 1. Tipos algebraicos

### Ejercicio 1

Explique por que:

```text
a x (b + c)
```

es isomorfo a:

```text
(a x b) + (a x c)
```

Debe dar:

- una funcion de ida
- una funcion de vuelta
- una justificacion de por que no se pierde informacion

### Ejercicio 2

Determine si:

```text
a + (b x c)
```

es isomorfo a:

```text
(a + b) x (a + c)
```

Si no lo es, de un contraejemplo por cardinalidad.

## 2. Recursion de cola

### Ejercicio 3

Complete:

```prolog
invierte(L, R) :-
    invierte(L, [], R).

invierte([], Acc, Acc).
invierte([H|T], Acc, R) :-
    % completar
```

### Ejercicio 4

Trace mentalmente:

```prolog
?- invierte([a,b,c], X).
```

indicando el valor del acumulador en cada paso.

## 3. Ambito y binding

### Ejercicio 5

Responda si o no:

1. Python tiene ambito dinamico?
2. Python tiene binding dinamico?
3. C es de tipos estrictos?
4. C es de tipos estaticos?
5. Haskell tiene binding dinamico?

### Ejercicio 6

Explique con sus palabras la diferencia entre:

- ambito lexico
- ambito dinamico
- binding

## 4. Tipado

### Ejercicio 7

Clasifique:

- Python
- JavaScript
- Rust
- C

en la matriz:

- fuerte o debil
- estatico o dinamico

### Ejercicio 8

De un ejemplo corto de codigo que muestre por que JavaScript se considera mas debilmente tipado que Python.

## 5. Integracion

### Ejercicio 9

Explique por que los tipos algebraicos permiten razonar "como si los tipos fueran numeros", pero el isomorfismo depende de que exista conversion ida y vuelta sin perdida de informacion.

### Ejercicio 10

Explique por que recursion de cola, ambito y sistema de tipos son tres temas distintos, aunque todos afectan la ejecucion real de un programa.
