# Guia de Prolog: CSP y Prolog vs Datalog

Esta guia complementa los apuntes anteriores con dos temas muy importantes:

- problemas de satisfaccion de restricciones
- diferencias entre Prolog y Datalog

La idea es conectar la intuicion de backtracking de Prolog con las garantias teoricas de Datalog.

## 1. Que es un CSP

Un `CSP` es un problema de satisfaccion de restricciones.

La idea general es:

```text
Variables:      cosas que no sabe
Dominios:       valores posibles para cada variable
Restricciones:  reglas que deben cumplirse
```

### Ejemplo intuitivo

En un problema de coloreo de mapas:

- variables: el color de cada pais
- dominio: `rojo`, `verde`, `azul`
- restricciones: paises vecinos no pueden tener el mismo color

Resolver el problema significa:

- asignar valores a todas las variables
- de modo que todas las restricciones se cumplan

## 2. Generate-and-test

La estrategia mas simple para resolver un CSP es:

1. generar candidatos
2. probar si cumplen las restricciones
3. si fallan, retroceder
4. si sirven, continuar

Esto es exactamente la intuicion de resolver un sudoku a mano:

```text
1. pone un valor
2. revisa si rompe una regla
3. si rompe, borra y prueba otro
4. si no rompe, sigue
5. si llena todo correctamente, encontro solucion
```

En Prolog, esto aparece de forma natural gracias a:

- unificacion
- backtracking
- orden de metas

## 3. Ejemplo: coloreo de mapa

Suponga 4 paises `A`, `B`, `C` y `D`.

Dominio:

```prolog
color(rojo).
color(verde).
color(azul).
```

Programa:

```prolog
colorear(A, B, C, D) :-
    color(A), color(B), color(C), color(D),
    A \= B,
    A \= C,
    B \= C,
    B \= D,
    C \= D.
```

### Lectura declarativa

`A`, `B`, `C` y `D` forman una coloracion valida si:

- cada uno es un color permitido
- y las parejas vecinas tienen colores distintos

### Lectura procedimental

Prolog hace algo conceptualmente parecido a esto:

```text
Prueba A=rojo, B=rojo  -> A \= B falla -> backtracking
Prueba A=rojo, B=verde -> ok
  Prueba C=rojo        -> A \= C falla -> backtracking
  Prueba C=verde       -> B \= C falla -> backtracking
  Prueba C=azul        -> ok
    Prueba D=rojo      -> B \= D ok, C \= D ok -> solucion
```

Esto muestra que Prolog explora un arbol de posibilidades y poda ramas cuando una restriccion falla.

## 4. El orden importa muchisimo

Considere estas dos versiones.

### Version lenta

```prolog
colorear(A, B, C, D) :-
    color(A), color(B), color(C), color(D),
    A \= B, B \= C, C \= D.
```

### Version mas rapida

```prolog
colorear(A, B, C, D) :-
    color(A),
    color(B), A \= B,
    color(C), A \= C, B \= C,
    color(D), B \= D, C \= D.
```

## 5. Por que la segunda version es mejor

En la version lenta:

- primero se generan muchas combinaciones
- luego se revisan las restricciones

En la version rapida:

- apenas se genera un valor, se chequean restricciones relevantes
- si falla, se corta la rama de inmediato

Esto se llama `poda`.

### Intuicion visual

```text
Version lenta:
  genera [rojo, rojo, rojo, rojo] -> revisa -> falla
  genera [rojo, rojo, rojo, verde] -> revisa -> falla
  ...

Version rapida:
  A=rojo
  B=rojo -> A \= B falla -> ni prueba C ni D
  B=verde -> ok
    C=rojo -> A \= C falla -> ni prueba D
    C=azul -> ok
      D=...
```

## 6. Leccion clave de CSP en Prolog

Para escribir buenos programas generate-and-test:

- genere valores
- verifique restricciones lo antes posible
- corte ramas temprano

Eso no cambia el significado logico del programa, pero si puede cambiar mucho su rendimiento.

## 7. Diferencia fundamental

La idea central puede resumirse asi:

```text
Prolog   -> mas expresivo -> puede no terminar
Datalog  -> mas restringido -> siempre termina
```

Es un intercambio entre:

- poder expresivo
- garantias teoricas

## 8. Tabla comparativa

```text
+-----------------+------------------+------------------+
|                 | Prolog           | Datalog          |
+-----------------+------------------+------------------+
| Terminacion     | no garantizada   | siempre termina  |
| Listas/arboles  | si               | no               |
| Recursion inf.  | posible          | imposible        |
| Negacion        | por falla        | estratificada    |
| Uso tipico      | IA, parsing      | bases de datos   |
+-----------------+------------------+------------------+
```

## 9. Por que Prolog puede no terminar

En Prolog se pueden escribir reglas problematicas como:

```prolog
camino(X, Z) :- camino(X, Y), camino(Y, Z).
```

Procedimentalmente, esto puede llevar a recursion infinita.

Como Prolog es muy expresivo, entra en la misma zona conceptual del `Halting Problem`:

- no siempre se puede saber de antemano si una computacion terminara

## 10. Por que Datalog si termina

Datalog impone restricciones importantes.

Por ejemplo:

- no permite functores arbitrarios
- no permite listas como `[H|T]`
- trabaja sobre dominios finitos derivados de hechos base

Eso hace que la evaluacion sea decidible y que las consultas terminen.

## 11. Seguridad en Datalog

Una regla es `segura` cuando las variables quedan acotadas por un conjunto finito derivado del cuerpo.

Una forma clasica de inseguridad es poner variables en la cabeza o en una negacion sin haberlas limitado antes.

### Ejemplo inseguro

```prolog
enemigo(X, Y) :- not amigo(X, Y).
```

Problema:

- `X` y `Y` no estan acotadas por hechos positivos finitos
- podrian representar cualquier elemento del universo

### Ejemplo seguro

```prolog
enemigo(X, Y) :- persona(X), persona(Y), not amigo(X, Y).
```

Aqui:

- `X` viene de `persona(X)`
- `Y` viene de `persona(Y)`

Entonces ambas variables quedan restringidas a un conjunto finito conocido.

## 12. Estratificacion

La negacion en Datalog se maneja con `estratificacion`.

La idea es organizar los predicados por capas:

- primero se calculan las capas base
- luego capas que dependen positivamente de las anteriores
- despues capas que pueden negar predicados ya calculados

### Ejemplo problematico

```prolog
a :- not b.
b :- not a.
```

Esto genera una dependencia circular con negacion.

No hay una capa clara que permita evaluar una antes que la otra.

### Ejemplo estratificado

```prolog
amigo(ana, bob).
conocido(X, Y) :- amigo(X, Y).
enemigo(X, Y) :- persona(X), persona(Y), not amigo(X, Y).
```

Aqui una posible organizacion es:

- estrato 1: `amigo/2`, `persona/1`
- estrato 2: `conocido/2`
- estrato 3: `enemigo/2`

La negacion solo mira hacia atras, no hacia su mismo nivel.

## 13. Conexion con Halting Problem

La intuicion general es:

```text
Prolog:
  mas poder
  mas expresividad
  menos garantias

Datalog:
  menos poder
  menos expresividad
  mas garantias
```

Datalog escapa a varios problemas de terminacion precisamente porque renuncia a parte del poder de Prolog.

## 14. Resumen general

```text
CSP:
  CSP = variables + dominios + restricciones
  generate-and-test = generar y probar
  orden importa = restricciones tempranas podan antes

Prolog vs Datalog:
  Prolog vs Datalog = expresividad vs garantias
  seguridad = variables acotadas por conjuntos finitos
  estratificacion = negacion organizada en capas
```

## 15. Ejercicio 3. Variables seguras e inseguras en Datalog

Considere:

```datalog
ocioso(Estudiante) :- pertenece(Estudiante, Universidad), not(matriculado(Estudiante, Curso)).
```

### Analisis

Variables:

- `Estudiante`
- `Universidad`
- `Curso`

### Seguridad

- `Estudiante` es segura porque aparece en el literal positivo `pertenece(Estudiante, Universidad)`
- `Universidad` es segura porque aparece en el literal positivo `pertenece(Estudiante, Universidad)`
- `Curso` es insegura porque solo aparece dentro de una negacion y no esta acotada por ningun literal positivo

### Conclusion

La regla, tal como esta escrita, es insegura.

### Version segura posible

```datalog
ocioso(Estudiante) :-
    pertenece(Estudiante, Universidad),
    curso(Curso),
    not(matriculado(Estudiante, Curso)).
```

Nota:

Esta version hace segura a `Curso`, aunque semanticamente puede no capturar exactamente la nocion intuitiva de "ocioso". En muchos casos conviene reformular el predicado segun la intencion precisa del dominio.

## 16. Ejercicio 4. Predicados intensionales y extensionales

Considere:

```datalog
hijo(juan, alberto).
hijo(alberto, pedro).
hijo(juan, alfredo).
nieto(X,Y) :- hijo(X,Z), hijo(Z,Y).
```

### EDB

La base de datos extensional contiene los hechos base.

Entonces:

- `hijo/2` pertenece a la `EDB`

### IDB

La base de datos intensional contiene los predicados definidos por reglas.

Entonces:

- `nieto/2` pertenece a la `IDB`

## 17. Ejercicio 5. Diagrama de estratos

Considere:

```datalog
compuesto(N) :- numero(N), not(primo(N)).
```

Suponiendo que `primo/1` es un predicado extensional.

### Dependencias

- `compuesto/1` depende positivamente de `numero/1`
- `compuesto/1` depende negativamente de `primo/1`

Si `numero/1` y `primo/1` son EDB, entonces ambos pueden ir en un estrato base.

### Diagrama de estratos

```text
Estrato 1:
  numero/1
  primo/1

Estrato 2:
  compuesto/1   depende de numero/1 y not primo/1
```

### Idea

Primero se conocen los numeros y los primos.
Despues se calcula `compuesto/1` negando algo que ya fue determinado en un estrato anterior.

## 18. Ideas clave para estudiar

- CSP significa buscar valores que satisfacen restricciones
- Prolog resuelve muchos CSP con generate-and-test y backtracking
- el orden de las restricciones afecta mucho el rendimiento
- Datalog sacrifica expresividad para garantizar terminacion
- una variable debe quedar acotada por literales positivos para ser segura
- la negacion en Datalog requiere estratificacion
- EDB son hechos base; IDB son predicados definidos por reglas
