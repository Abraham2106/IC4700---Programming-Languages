# Guia de Tipos de Datos Algebraicos

Esta guia resume la intuicion algebraica de los tipos y como razonar sobre suma, producto e isomorfismos.

Temas cubiertos:

- suma de tipos
- producto de tipos
- isomorfismo entre tipos
- distributividad
- contraejemplos de no-isomorfismo

## 1. Idea general

En muchos lenguajes funcionales, los tipos pueden verse como si fueran expresiones algebraicas.

Ejemplos:

- `Either a b` representa una suma: `a + b`
- `(a, b)` representa un producto: `a x b`
- `Maybe a` representa `1 + a`

La intuicion es contar cuantas formas distintas hay de construir un valor.

Esto no quiere decir que los tipos sean literalmente numeros, sino que su estructura se comporta de forma parecida a la aritmetica cuando los analizamos por habitantes y constructores.

Por ejemplo:

- un tipo suma agrega alternativas
- un tipo producto combina elecciones
- un tipo con un solo habitante se comporta como `1`
- un tipo sin habitantes se comporta como `0`

Esto no significa que los tipos sean literalmente numeros, sino que bajo ciertas condiciones podemos razonar sobre su estructura de una forma parecida a la aritmetica.

La razon es esta:

- un tipo suma ofrece una eleccion entre alternativas
- un tipo producto exige tener ambas partes
- un tipo sin habitantes se comporta como `0`
- un tipo con un solo habitante se comporta como `1`

Por eso aparecen analogias como:

```text
Bool  ~=  1 + 1
Maybe a  ~=  1 + a
Either a b  ~=  a + b
(a, b)  ~=  a x b
```

## 2. Suma de tipos

Un tipo suma representa una eleccion entre alternativas.

Ejemplo:

```haskell
data Either a b = Left a | Right b
```

Esto significa:

- o tenemos un valor de tipo `a`
- o tenemos un valor de tipo `b`

Por eso se interpreta como:

```text
a + b
```

Si `a` tiene `m` valores posibles y `b` tiene `n`, entonces `Either a b` tiene `m + n` valores posibles, porque los constructores `Left` y `Right` mantienen separadas las dos ramas.

## 3. Producto de tipos

Un tipo producto representa que ambos componentes existen al mismo tiempo.

Ejemplo:

```haskell
data Par a b = Par a b
```

o equivalentemente una tupla:

```haskell
(a, b)
```

Esto se interpreta como:

```text
a x b
```

porque un valor del producto contiene:

- un valor de `a`
- y un valor de `b`

Si `a` tiene `m` habitantes y `b` tiene `n`, entonces el producto tiene `m x n` habitantes, porque cada valor de `a` puede emparejarse con cada valor de `b`.

## 4. Isomorfismo de tipos

Dos tipos `A` y `B` son isomorfos si existen funciones:

```text
f : A -> B
g : B -> A
```

tal que:

```text
g(f(x)) = x
f(g(y)) = y
```

La idea es que ambos tipos tienen exactamente la misma informacion, aunque empaquetada de forma distinta.

Esta ultima parte es importante: no basta con que dos tipos "se parezcan". Para que exista isomorfismo:

- toda informacion del primer tipo debe poder traducirse al segundo
- y luego recuperarse exactamente
- sin perdida
- sin ambiguedad

Si una conversion comprime varios valores distintos en uno solo, entonces no hay isomorfismo.

Ese punto es central: isomorfismo no es "se parecen" ni "tienen una intuicion parecida". Isomorfismo significa que se puede ir y volver sin perder informacion y sin introducir ambiguedad.

## 4.1. Por que importa el conteo de habitantes

Una herramienta muy util es contar habitantes cuando los tipos son finitos.

Si dos tipos finitos tienen diferente numero de habitantes:

- no pueden ser isomorfos

Esto no prueba por si solo que dos tipos con la misma cardinalidad sean isomorfos en todos los contextos, pero si descarta rapidamente muchos falsos candidatos.

Ejemplo:

```text
Bool tiene 2 habitantes: True, False
Maybe Bool tiene 3 habitantes: Nothing, Just True, Just False
```

Entonces:

```text
Bool  no es isomorfo a  Maybe Bool
```

porque `2 /= 3`.

## 5. Demostracion de

```text
a x (b + c)  ~=  (a x b) + (a x c)
```

Queremos mostrar que:

```haskell
(a, Either b c)
```

es isomorfo a:

```haskell
Either (a, b) (a, c)
```

## 6. Funcion de ida

Definimos:

```haskell
f :: (a, Either b c) -> Either (a, b) (a, c)
f (x, Left y)  = Left (x, y)
f (x, Right z) = Right (x, z)
```

Intuicion:

- si el segundo componente trae un `b`, devolvemos `(a, b)`
- si trae un `c`, devolvemos `(a, c)`

## 7. Funcion de vuelta

Definimos:

```haskell
g :: Either (a, b) (a, c) -> (a, Either b c)
g (Left (x, y))  = (x, Left y)
g (Right (x, z)) = (x, Right z)
```

## 8. Verificacion del isomorfismo

Si aplicamos `g` despues de `f`:

```text
g(f(x, Left y))   = g(Left (x, y))   = (x, Left y)
g(f(x, Right z))  = g(Right (x, z))  = (x, Right z)
```

Entonces:

```text
g . f = id
```

Y si aplicamos `f` despues de `g`:

```text
f(g(Left (x, y)))   = f(x, Left y)   = Left (x, y)
f(g(Right (x, z)))  = f(x, Right z)  = Right (x, z)
```

Entonces:

```text
f . g = id
```

Por lo tanto:

```text
a x (b + c)  ~=  (a x b) + (a x c)
```

## 9. Segunda pregunta:

```text
a + (b x c)   ?=   (a + b) x (a + c)
```

La respuesta es:

```text
no, no son isomorfos en general
```

## 10. Por que no

Comparemos la cantidad de informacion.

Lado izquierdo:

```text
a + (b x c)
```

significa:

- o un valor de `a`
- o un par `(b, c)`

Lado derecho:

```text
(a + b) x (a + c)
```

significa:

- una eleccion entre `a` y `b`
- y al mismo tiempo una eleccion entre `a` y `c`

Eso crea combinaciones extra.

Otra forma de verlo es esta:

en `a + (b x c)` hay dos escenarios globales:

1. estamos en la rama `a`
2. estamos en la rama `(b, c)`

En cambio, en `(a + b) x (a + c)` hay dos decisiones separadas:

1. una para el primer componente
2. otra para el segundo componente

Eso produce independencia entre elecciones que el tipo izquierdo no tiene.

## 11. Contraejemplo por cardinalidad

Suponga:

```text
|a| = 1
|b| = 1
|c| = 1
```

Entonces:

```text
|a + (b x c)| = 1 + (1 x 1) = 2
```

Pero:

```text
|(a + b) x (a + c)| = (1 + 1) x (1 + 1) = 4
```

Como `2 /= 4`, no pueden ser isomorfos en general.

Este tipo de argumento por cardinalidad es muy util en examen porque permite descartar rapido falsos isomorfismos sin tener que intentar escribir funciones imposibles.

## 12. Intuicion del fallo

En:

```text
(a + b) x (a + c)
```

pueden aparecer combinaciones como:

- `(Left a, Left a)`
- `(Left a, Right c)`
- `(Right b, Left a)`
- `(Right b, Right c)`

Muchas de esas combinaciones no corresponden a una unica forma natural de representar:

```text
a + (b x c)
```

En el lado derecho hay dos decisiones parciales independientes; en el izquierdo hay una sola decision global. Esa diferencia estructural explica por que no hay distributividad hacia ese lado.

## 13. Conexion con programacion

Esto importa porque permite:

- disenar tipos mejor
- detectar equivalencias
- transformar estructuras sin perder informacion
- razonar sobre APIs y modelos de datos

Tambien ayuda a entender por que algunos diseños de tipos son mejores que otros.

Por ejemplo, si un resultado puede estar en uno de varios estados mutuamente excluyentes, suele ser mejor modelarlo con una suma:

```haskell
data Resultado = Exito Valor | Error Mensaje
```

que con varios campos separados, porque el tipo suma hace explicitas las alternativas validas del sistema.

Eso mejora mucho el diseño de APIs porque elimina estados imposibles desde el propio sistema de tipos.

## 14. Ideas clave para estudiar

- `Either a b` es una suma
- `(a, b)` es un producto
- dos tipos son isomorfos si pueden convertirse ida y vuelta sin perder informacion
- `a x (b + c)` distribuye sobre la suma
- `a + (b x c)` no es isomorfo a `(a + b) x (a + c)` en general
