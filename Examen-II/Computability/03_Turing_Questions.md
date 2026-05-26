# Preguntas Guiadas: Turing, Halting y Universalidad

Este archivo resume respuestas directas a las preguntas conceptuales mas probables.

## 1. Maquina H y contradiccion

Suponga una maquina `H` que, dada una codificacion `<M, C>`, responde:

- `Si` si `M` con `C` termina
- `No` si `M` con `C` no termina

La contradiccion aparece porque, si `H` existiera, podriamos construir una maquina `D` que use a `H` para hacer lo contrario de lo que `H` predice sobre `D` misma.

Construccion:

1. `D(x)` consulta a `H` sobre `<D, x>`
2. si `H` dice "termina", `D` entra en loop
3. si `H` dice "no termina", `D` termina

Ahora evaluamos `D(D)`:

- si `H` dice que `D(D)` termina, entonces `D(D)` no termina
- si `H` dice que `D(D)` no termina, entonces `D(D)` termina

En ambos casos hay contradiccion.

Conclusion:

- `H` no puede existir
- el problema de halting es indecidible

## 2. Que significa que un lenguaje sea Turing universal

Un lenguaje es **Turing universal** si puede expresar cualquier computacion que una maquina de Turing pueda realizar.

Equivalentemente:

- puede simular una maquina de Turing universal
- puede implementar cualquier algoritmo computable

Esto lo hace un lenguaje de proposito general desde el punto de vista de computabilidad.

## 3. Que lenguajes no deben ser Turing universales

Hay contextos donde no conviene que un lenguaje sea Turing universal.

Ejemplos:

- lenguajes de consultas a bases de datos
- lenguajes de reglas con garantias de terminacion
- lenguajes de configuracion
- DSL para validacion o analisis automatico

## 4. Por que no conviene

Si el lenguaje no es Turing universal, suele ser mas facil garantizar:

- terminacion
- decidibilidad
- analisis estatico
- optimizacion segura

## 5. Ejemplos concretos

- `Datalog` es un ejemplo clasico de lenguaje que restringe expresividad para garantizar terminacion
- muchos lenguajes de consulta o reglas hacen algo parecido

## 6. Conexion general

La idea profunda es esta:

- mas expresividad suele traer menos garantias
- menos expresividad puede traer mas control teorico

Por eso, no todos los lenguajes deberian aspirar a ser completamente universales.
