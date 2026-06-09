# Prolog y Datalog — Expresividad contra Garantías Teóricas

Prolog es extremadamente poderoso (es Turing-completo), pero su evaluación de búsqueda de profundidad (DFS) en estructuras recursivas infinitas lo expone al Problema de la Parada (Halting Problem). Datalog surge como un derivado relacional estricto que sacrifica poder de abstracción para garantizar resoluciones matemáticas deterministas y siempre finitas.

---

## 1. Datalog y Restricciones de Dominio Finito

En Datalog, el orden lógico en que se declaran las cláusulas **no importa**. Un programa es concebido estrictamente como un conjunto estático de ecuaciones lógicas, en contraste con la lista estructurada dependiente del orden de Prolog.

### 1.1 Funciones y Listas Prohibidas

Para que Datalog pueda garantizar que el árbol de inferencia sea invariablemente finito (eliminando bucles recursivos como $f(f(f(x)))$), prohíbe terminantemente la declaración de functores lógicos con aridad mayor a 0 (Arity > 0).

Esto implica la desintegración formal de las estructuras de datos complejas:
- No existen functores compuestos: unificar `append(cons(A, []), [])` es lógicamente inadmisible en Datalog.
- La inexistencia de functores impide usar listas recursivas estilo `[H|T]`.
- Todo término está limitado a ser una variable `X` o una constante inerte `c`. El dominio se vuelve finito por obligación y no generativo.

### 1.2 Bases de Datos Extensionales vs Intensionales

Los programas de Datalog mapean orgánicamente a arquitecturas de Bases de Datos Relacionales (RDBMS) separando estrictamente la definición de un problema en dos conjuntos lógicos:
- **EDB (Extensional Database):** Contiene exclusivamente las realidades base no derivadas, es decir, hechos planos (como `empleado(juan, 100).`). Actúa análogamente a las tablas y tuplas insertadas en SQL.
- **IDB (Intensional Database):** Contiene los predicados derivados que dependen o condicionan a otros atributos, es decir, las Reglas puras (como `pagar(X, Y) :- empleado(X, Y).`). Funciona similar a las Vistas Dinámicas (Views) en SQL.

---

## 2. Variables Seguras (Safety)

Para evitar que Datalog asigne universos infinitos a una ecuación deductiva, se imponen restricciones draconianas sobre las variables de inferencia, garantizando la convergencia total (seguridad).

Una regla está "segura" si y solo si todas las variables proyectadas en la cabeza, o negadas en el cuerpo, aparecen previamente ancladas a una declaración literal afirmativa de dominio acotado.

**Ejemplo de Regla Insegura:**
```datalog
enemigo(X, Y) :- not amigo(X, Y).
```
El motor relacional colapsaría porque $X$ e $Y$ no tienen fronteras físicas; el motor debería probar todo el universo de constantes existentes (o infinitas) para verificar quién *no es* amigo de quién.

**Ejemplo de Regla Segura:**
```datalog
enemigo(X, Y) :- persona(X), persona(Y), not amigo(X, Y).
```
Aquí la evaluación de las variables se encajona: $X$ e $Y$ están acotados en longitud al tamaño finito de las relaciones (EDB) afirmadas en `persona/1`.

---

## 3. Negación por Falla y Estratificación

La "Negación por Falla" (Negation as Failure) en Prolog asume erróneamente que, si el motor es incapaz de probar deductivamente un hecho de la base de datos (debido al orden DFS subyacente), entonces tal hecho es lógicamente falso. Esto genera inconsistencias severas en recursividad cíclica:

```prolog
a :- not b.
b :- not a.
```
Si evaluamos $a$, llamaría iterativamente y ad nauseam a $b$ y viceversa, sin un estrato base matemático claro (Paradoja de Russell).

Para sanear esto, Datalog impone el rigor de **Estratificación**. 
Los predicados deben estructurarse por "Capas" de dependencia de abstracción lógicas:
1. Una regla puede depender deductiva o negativamente solo de un predicado posicionado en la capa o estrato base inmediatamente inferior.
2. Un estrato que utilice negación debe aguardar estrictamente a que su dependencia inferior se haya calculado y cerrado en tuplas completas, invalidando la negación sobre su mismo nivel lógico para prohibir auto-referencias negativas insolubles.
