# Ejercicios de Tipos, Recursion de Cola y Ambito

Esta practica reune ejercicios cortos sobre los temas de la carpeta.

Sugerencia de uso:

- responda primero sin apoyo
- luego use las otras guias para mejorar la justificacion
- trate de escribir no solo la etiqueta final, sino tambien el por que

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

Pista:

- el valor de tipo `a` se conserva entero en ambos lados
- la unica decision real es si el segundo componente venia de la rama `b` o de la rama `c`

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

Pista:

- pruebe con cardinalidades pequenas como `1, 1, 1`
- si el numero de habitantes no coincide, ya no puede haber isomorfismo

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

Pista:

- el acumulador debe guardar la inversion parcial
- cada cabeza nueva debe agregarse al frente del acumulador

### Ejercicio 4

Trace mentalmente:

```prolog
?- invierte([a,b,c], X).
```

indicando el valor del acumulador en cada paso.

Una buena respuesta debe mostrar:

- llamada inicial
- llamadas intermedias
- caso base
- valor final

## 3. Ambito y binding

### Ejercicio 5

Responda si o no:

1. Python tiene ambito dinamico?
2. Python tiene binding dinamico?
3. C es de tipos estrictos?
4. C es de tipos estaticos?
5. Haskell tiene binding dinamico?

Sugerencia:

- escriba una linea corta de justificacion debajo de cada respuesta

### Ejercicio 6

Explique con sus palabras la diferencia entre:

- ambito lexico
- ambito dinamico
- binding

Pista:

- "ambito" responde donde puede verse el nombre
- "binding" responde con que entidad concreta se asocia

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

Idealmente, justifique cada clasificacion con un microejemplo.

### Ejercicio 8

De un ejemplo corto de codigo que muestre por que JavaScript se considera mas debilmente tipado que Python.

Pista:

- busque una operacion con coercion implicita en JavaScript
- comparela con una operacion equivalente que falle en Python

## 5. Integracion

### Ejercicio 9

Explique por que los tipos algebraicos permiten razonar "como si los tipos fueran numeros", pero el isomorfismo depende de que exista conversion ida y vuelta sin perdida de informacion.

Una buena respuesta debe mencionar:

- habitantes
- suma y producto de tipos
- funciones de ida y vuelta
- ausencia de perdida de informacion

### Ejercicio 10

Explique por que recursion de cola, ambito y sistema de tipos son tres temas distintos, aunque todos afectan la ejecucion real de un programa.

Pista:

- recursion de cola trata de forma de evaluacion y uso de pila
- ambito y binding tratan de nombres y visibilidad
- tipos tratan de valores y operaciones validas

## Criterio de estudio

Si una respuesta cabe en una sola linea, probablemente todavia esta debil. En este bloque vale mucho mas una respuesta corta pero justificada que una etiqueta memorizada sin razon.
