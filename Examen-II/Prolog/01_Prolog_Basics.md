# Guia basica de Prolog

## Programacion logica
La programacion logica tiene sus raices en el *automated theorem proving* (demostracion automatizada de teoremas). A diferencia de la demostracion de teoremas pura, la programacion logica usa el marco formal de una logica para **especificar** y **ejecutar** computaciones.

En general, la programacion logica trabaja sobre fragmentos bien comportados de la logica, de forma que los programas no solo expresan conocimiento, sino que tambien pueden ejecutarse.

Podemos pensar un programa logico de dos maneras:

* **Interpretacion declarativa**:
  El programa declara **que** se quiere computar.
* **Interpretacion procedimental**:
  El programa describe **como** ocurre la computacion.

La ventaja de la interpretacion declarativa es que permite razonar sobre la correccion del programa sin depender tanto del mecanismo de ejecucion. Sin embargo, en la practica, para que un programa en Prolog sea eficiente y termine correctamente, tambien hay que entender su comportamiento procedimental.

## Prolog
Prolog es uno de los lenguajes mas representativos de la programacion logica. Fue desarrollado a inicios de los anos 70, originalmente con interes en procesamiento de lenguaje natural.

### Sintaxis basica

- **Variables**:
  `X`, `Y`, `Z` pertenecen al conjunto de variables.
- **Simbolos de predicado**:
  `p`, `q`, `r` pertenecen al conjunto de simbolos de predicado.
- **Simbolos de funcion**:
  `f`, `g`, `h`, `a`, `b`, `c` pertenecen al conjunto de simbolos de funcion.
- **Terminos**:
  $s, t ::= X \mid f(t_1, \ldots, t_n)$
- **Atomos**:
  $A, H ::= p(t_1, \ldots, t_n)$

Cada simbolo de predicado y cada simbolo de funcion tiene una **aridad fija**, es decir, un numero fijo de argumentos.

Una **constante** es un simbolo de funcion de aridad `0`. Por ejemplo, `alice` y `bob` son constantes.

Si el predicado `child` tiene aridad `2`, entonces:

- `child(alice, bob)` es un atomo bien formado.
- `child(alice)` **no** es valido, porque no respeta la aridad.

Un **termino ground** (o termino a tierra) es un termino sin variables. Un **atomo ground** es un atomo sin variables.

Importante: en Prolog, los simbolos de funcion y los simbolos de predicado **no tienen significado implicito** por si mismos. Su significado depende de las clausulas del programa.

### Unificacion
Las variables funcionan como *placeholders* para terminos.

Una **sustitucion** es un mapeo finito de variables a terminos. Por ejemplo:

- $\sigma = \{X \mapsto alice,\; Y \mapsto bob\}$

Aplicar una sustitucion significa reemplazar las variables por los terminos correspondientes.

Decimos que una sustitucion $\sigma$ **unifica** dos terminos $s$ y $t$ si:

$$
\sigma(s) = \sigma(t)
$$

La **unificadora mas general** (*most general unifier*, MGU) es la sustitucion mas general que logra hacer iguales ambos terminos. Se llama "mas general" porque cualquier otra unificadora puede obtenerse extendiendo esa sustitucion.

Ejemplos:

- `f(X)` y `f(alice)` unifican con `{ X -> alice }`
- `pair(X, bob)` y `pair(alice, Y)` unifican con `{ X -> alice, Y -> bob }`
- `f(X, Y)` y `g(alice)` **no** unifican, porque los simbolos principales son distintos

Cuando no existe una sustitucion que haga coincidir dos terminos, decimos que la unificacion **falla**.

La unificacion es central en Prolog porque los resultados de una computacion suelen expresarse precisamente como sustituciones.

### Clausulas
Una **clausula** tiene la forma:

$$
A_0 \; \texttt{:-} \; A_1, \ldots, A_m.
$$

Donde:

- $A_0$ es la **cabeza** (*head*) de la clausula.
- $A_1, \ldots, A_m$ es el **cuerpo** (*body*) de la clausula.

Si el cuerpo es vacio, simplemente se escribe:

$$
A_0.
$$

Intuitivamente, una clausula significa:

"$A_0$ es verdadero si $A_1, \ldots, A_m$ son verdaderos".

Visto como formula logica:

$$
\forall X_1, \ldots, X_k.\; (A_1 \land \cdots \land A_m) \Rightarrow A_0
$$

donde $X_1, \ldots, X_k$ son las variables que aparecen en la clausula.

Un **programa logico** es una coleccion de clausulas. El programa se activa con una **consulta** (*query*), que normalmente es un atomo o una secuencia de atomos.

Una **solucion** a una consulta es una sustitucion $\sigma$ tal que $\sigma(A)$ resulta verdadera.

### Hechos y reglas
En Prolog, una clausula puede verse de dos maneras practicas:

- **Hecho**:
  una clausula sin cuerpo.
- **Regla**:
  una clausula cuyo cuerpo contiene una o varias metas.

Ejemplos:

```prolog
male(lot).
father(haran, lot).

son(X, Y) :- father(Y, X), male(X).
```

Lectura declarativa:

- `male(lot).` significa que `lot` es hombre.
- `father(haran, lot).` significa que `haran` es padre de `lot`.
- `son(X, Y) :- father(Y, X), male(X).` significa que `X` es hijo de `Y` si `Y` es padre de `X` y `X` es hombre.

Un **programa logico** puede definirse formalmente como un conjunto finito de reglas. En Prolog real, ademas, el orden en que aparecen importa para la ejecucion.

### Consecuencia logica
Una consulta existencial `G` es **consecuencia logica** de un programa `P` si puede deducirse a partir de las clausulas de `P`.

Una forma de verlo es esta:

- si existe una instancia de una regla `A :- B1, ..., Bn`
- y `B1, ..., Bn` tambien son consecuencia logica del programa
- entonces $A$ tambien lo es

En el caso base, un hecho como:

```prolog
male(lot).
```

ya es una consecuencia logica inmediata del programa.

Dicho de otra forma: una meta `G` es consecuencia logica de `P` si puede obtenerse mediante un numero finito de pasos de deduccion.

### Modus ponens universal
Para incorporar reglas al proceso de deduccion, se usa una generalizacion del **modus ponens**.

Recordatorio:

- del modus ponens clasico, a partir de $B$ y $A \leftarrow B$, se deduce $A$

En programacion logica usamos una version mas general:

- si $A' \leftarrow B_1', \ldots, B_n'$ es una instancia de una regla del programa
- y $B_1', \ldots, B_n'$ ya fueron deducidos
- entonces puede deducirse $A'$

Esta regla se conoce como **modus ponens universal**.

Es importante porque conecta directamente la lectura logica del programa con su ejecucion: responder una consulta consiste, en esencia, en aplicar repetidamente esta regla.

### Ejemplo: append/3
Considere el siguiente programa que define la concatenacion de listas:

```prolog
append([], Y, Y).
append([H|T], Y, [H|U]) :- append(T, Y, U).
```

Aqui:

- `[]` representa la lista vacia.
- `[H|T]` representa una lista con cabeza `H` y cola `T`.

Interpretacion:

- La concatenacion de la lista vacia con `Y` es `Y`.
- Si la primera lista empieza con `H`, entonces el resultado tambien empieza con `H`, y el resto se obtiene concatenando `T` con `Y`.

Ejemplos de consultas:

```prolog
?- append([alice], [bob], [alice, bob]).
true.
```

La solucion es la sustitucion vacia, porque la consulta ya coincide con el resultado esperado.

```prolog
?- append([alice], [bob], Z).
Z = [alice, bob].
```

```prolog
?- append([alice], Y, [bob]).
false.
```

La ultima consulta no tiene solucion porque no existe una lista $Y$ tal que $[alice] \mathbin{+\!\!+} Y = [bob]$.

### Ejemplo: relaciones familiares
Considere los siguientes hechos:

```prolog
father(haran, lot).
male(lot).
mother(sarah, isaac).
male(isaac).
```

Una primera definicion de `son/2` podria ser:

```prolog
son(X, Y) :- father(Y, X), male(X).
```

Con esta regla, la consulta:

```prolog
?- son(lot, haran).
true.
```

funciona correctamente porque se reduce a probar:

- `father(haran, lot)`
- `male(lot)`

Sin embargo, esta especificacion es **incompleta** si queremos capturar el significado intuitivo de "hijo", porque un hijo tambien puede ser hijo de su madre. Para eso agregamos:

```prolog
son(X, Y) :- mother(Y, X), male(X).
```

Todavia mejor, podemos introducir un predicado auxiliar:

```prolog
parent(X, Y) :- father(X, Y).
parent(X, Y) :- mother(X, Y).

son(X, Y) :- parent(Y, X), male(X).
grandparent(X, Y) :- parent(X, Z), parent(Z, Y).
```

Esta forma es mas compacta, mas elegante y mas facil de extender. Una coleccion de reglas con el mismo predicado en la cabeza, como las dos reglas de `parent/2`, se suele llamar un **procedimiento**.

### Ejemplo: chequeo de tipos
Un aspecto interesante de Prolog es que permite describir reglas logicas de manera muy directa. Por ejemplo, se puede modelar un sistema de tipos para un lambda calculo simple.

Suponga la siguiente sintaxis de expresiones:

- $n$
- $x$
- $\lambda x.\, e$
- $e_1\; e_2$
- $e_1 + e_2$

Una posible codificacion en terminos de Prolog es:

- $n$ se representa como `literal(n)`
- $x$ se representa como `var(x)`
- $\lambda x.\, e$ se representa como `lambda(x, T(e))`
- $e_1\; e_2$ se representa como `apply(T(e_1), T(e_2))`
- $e_1 + e_2$ se representa como `plus(T(e_1), T(e_2))`

Para representar tipos podemos usar:

- `int` para enteros
- `arrow(S, T)` para funciones de tipo $S \to T$

Si usamos el predicado `type(G, E, T)`, entonces significa:

"En el contexto `G`, la expresion `E` tiene tipo `T`".

Un contexto puede modelarse como una lista de pares `(variable, tipo)`.

Ejemplo de reglas:

```prolog
type(G, literal(X), int).
type(G, var(X), T) :- member([X, T], G).
type(G, apply(M, N), T) :- type(G, M, arrow(S, T)), type(G, N, S).
type(G, lambda(X, M), arrow(S, T)) :- type([[X, S]|G], M, T).
```

La idea importante aqui es que el mismo programa puede servir tanto para:

- **verificar** si una expresion esta bien tipada
- **inferir** tipos cuando estos no se conocen por completo

## Computacion en Prolog
Procedimentalmente, Prolog intenta resolver una consulta buscando clausulas cuya cabeza unifique con ella.

El proceso, a grandes rasgos, es:

1. Se inicia con la sustitucion vacia.
2. Se busca una clausula cuya cabeza pueda unificarse con la consulta actual.
3. Si la unificacion tiene exito, se extiende la sustitucion.
4. Luego se intenta satisfacer cada atomo del cuerpo de la clausula.
5. Si todo sale bien, se obtiene una solucion.
6. Si falla algun paso, Prolog hace **backtracking** y prueba otra alternativa.

### Interprete abstracto
Una forma de modelar la ejecucion de un programa logico es mediante un **interprete abstracto**.

Entrada:

- un programa `P`
- una consulta ground `Q`

Salida:

- `yes` si `Q` puede probarse a partir de `P`
- `no` si no puede probarse
- o ninguna respuesta si el proceso no termina

La idea central es mantener una lista actual de metas pendientes. A esa lista se le llama **resolvent**.

Esquema informal:

1. Inicializar el resolvent con la consulta.
2. Mientras el resolvent no este vacio:
3. Elegir una meta del resolvent.
4. Elegir una clausula cuya cabeza coincida con esa meta.
5. Reemplazar la meta elegida por el cuerpo de la clausula.
6. Si al final el resolvent queda vacio, la prueba tuvo exito.

En este contexto:

- una **reduccion** es el reemplazo de una meta por el cuerpo de una instancia de una regla
- las nuevas metas que aparecen son metas **derivadas**

Por ejemplo, para la consulta:

```prolog
?- son(lot, haran).
```

usando la regla:

```prolog
son(X, Y) :- father(Y, X), male(X).
```

la primera reduccion produce el resolvent:

```prolog
father(haran, lot), male(lot)
```

Si luego ambas metas se satisfacen con hechos, el resolvent queda vacio y la respuesta es `yes`.

### Arbol de prueba
Otra forma de visualizar una derivacion es mediante un **arbol de prueba**.

- la raiz es la consulta inicial
- cada nodo representa una meta reducida
- sus hijos son las metas derivadas al aplicar una regla

Para `son(lot, haran)`, el arbol se puede pensar asi:

```text
son(lot, haran)
|- father(haran, lot)
`- male(lot)
```

El numero de nodos del arbol da una idea del trabajo realizado durante la prueba.

### Orden de evaluacion
En teoria, la eleccion de clausulas puede verse como no determinista. Pero en Prolog real:

- las clausulas se consideran en el **orden en que fueron escritas**
- la evaluacion sigue una estrategia **depth-first search**

Esto significa que el orden del programa **si importa**.

Cuando una opcion falla, Prolog vuelve al ultimo **choice point** y prueba otra rama. Al hacerlo, descarta las sustituciones construidas despues de ese punto.

Hay dos elecciones importantes durante la ejecucion:

- **que meta reducir**
- **que clausula usar**

Teoricamente, el orden en que se reducen las metas no cambia si existe o no una prueba. Pero en la practica, bajo la estrategia de Prolog, el orden puede afectar mucho el rendimiento e incluso la terminacion.

La eleccion de clausulas es aun mas critica. Desde el punto de vista teorico, suele describirse como una eleccion **no determinista**: si existe alguna rama exitosa, se asume que puede elegirse. Prolog aproxima esta idea explorando alternativas con backtracking.

## Terminacion
Un programa de Prolog puede no terminar.

La terminacion de un programa recursivo depende mucho de:

- el orden de las clausulas
- el orden de los atomos en el cuerpo

Ejemplo que si termina:

```prolog
edge(alice, bob).

reach(X, X).
reach(X, Z) :- edge(X, Y), reach(Y, Z).
```

Consulta:

```prolog
?- reach(alice, Y).
```

Produce dos soluciones:

- `Y = alice`
- `Y = bob`

Ahora considere esta variante:

```prolog
edge(alice, bob).

reach(X, Z) :- reach(Y, Z), edge(X, Y).
reach(X, X).
```

Aunque logicamente parece parecida, procedimentalmente es muy distinta. En esta version, Prolog puede entrar en recursion infinita antes de llegar a una solucion.

Esto muestra una idea clave:

- **Interpretacion logica**: que significa el programa
- **Interpretacion procedimental**: como lo ejecuta Prolog

Ambas no siempre coinciden en comportamiento practico, especialmente en eficiencia y terminacion.

## Significado de un programa
Para poder decir si un programa "esta bien" o "dice lo que queriamos", necesitamos hablar de su significado.

El **significado** de un programa `P`, escrito como `M(P)`, es el conjunto de todas las metas ground que pueden deducirse a partir de `P`.

Esto tiene una consecuencia importante:

- si un programa contiene solo hechos ground, entonces su significado coincide con esos hechos
- si el programa contiene reglas, su significado tambien incluye todo lo que esas reglas implican

Por ejemplo, si el programa tiene:

```prolog
parent(X, Y) :- father(X, Y).
parent(X, Y) :- mother(X, Y).
```

entonces `M(P)` no solo contiene hechos sobre `father/2` y `mother/2`, sino tambien todos los hechos `parent(X, Y)` que puedan deducirse a partir de ellos.

### Correccion y completitud
Suponga que tenemos una idea de cual deberia ser el significado correcto del programa. A eso se le llama **significado intencional** o **intended meaning**.

Con eso podemos evaluar el programa de dos maneras:

- **Correcto**:
  `M(P)` es subconjunto del significado intencional.
  En otras palabras, el programa no deduce cosas incorrectas.
- **Completo**:
  el significado intencional es subconjunto de `M(P)`.
  Es decir, el programa deduce todo lo que deberia deducir.

Si ambas cosas se cumplen, entonces:

- el programa es correcto y completo
- o equivalentemente, `M(P)` coincide con el significado intencional

Ejemplo:

- si `son/2` solo usa la regla basada en `father/2`, el programa puede ser correcto, pero incompleto
- no deduce que `isaac` es hijo de `sarah`, aunque intuitivamente deberia hacerlo si sabemos que `sarah` es su madre
- al agregar la regla para `mother/2` o al definir `parent/2`, mejoramos la completitud

## Datalog
Datalog es otro lenguaje de programacion logica, emparentado con Prolog, pero con restricciones que le dan mejores propiedades de terminacion y complejidad.

### Diferencias principales con Prolog

- En Datalog, el orden de las clausulas **no importa**.
- Un programa de Datalog puede verse como un **conjunto** de clausulas, no como una lista ordenada.
- Las consultas sobre conjuntos finitos estan garantizadas a **terminar**.
- La evaluacion de programas Datalog es polinomial con respecto al tamano del programa.

### Restricciones de Datalog

#### 1. No se permiten funciones de aridad mayor que 0
Los terminos en Datalog solo pueden ser:

$$
s, t ::= X \mid c
$$

donde `X` es variable y `c` es constante.

Por eso, un termino estructurado como:

```prolog
append(cons(alice, []), [], Z)
```

puede ser valido en Prolog, pero **no** en Datalog.

#### 2. Toda variable de la cabeza debe aparecer en el cuerpo
Por ejemplo, esta clausula **no** es valida en Datalog:

```prolog
foo(X, Y) :- bar(X).
```

porque `Y` aparece en la cabeza, pero no en el cuerpo.

Estas restricciones reducen expresividad, pero permiten garantizar decidibilidad y terminacion en muchos casos.

## Ideas clave para estudiar

- Prolog combina una vision **declarativa** y una **procedimental**.
- La **unificacion** es el mecanismo fundamental de ejecucion.
- El **modus ponens universal** explica formalmente como se hacen deducciones a partir de reglas.
- Un programa se compone de **clausulas** y se ejecuta a partir de **consultas**.
- Los **hechos** son clausulas sin cuerpo; las **reglas** reducen una meta a nuevas submetas.
- El **resolvent** es el conjunto actual de metas pendientes.
- Una **reduccion** reemplaza una meta por el cuerpo de una regla aplicable.
- El **arbol de prueba** ayuda a visualizar como se construye una deduccion.
- El **orden** de reglas y metas en Prolog afecta terminacion y eficiencia.
- El significado `M(P)` de un programa es el conjunto de metas ground deducibles.
- Un programa ideal es **correcto** y **completo** con respecto a su significado intencional.
- Datalog es una variante mas restringida, pero con mejores garantias teoricas.
