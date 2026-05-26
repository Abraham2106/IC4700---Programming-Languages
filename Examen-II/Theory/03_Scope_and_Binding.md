# Guia de Ambito y Binding

Esta guia resume los conceptos de ambito, binding y algunas preguntas clasicas de examen sobre Python, Prolog, C y Haskell.

Temas cubiertos:

- ambito lexico y dinamico
- binding estatico y dinamico
- binding en Prolog
- respuestas conceptuales cortas

## 1. Ambito

El **ambito** o `scope` es la region del programa donde un nombre es visible y puede usarse.

La pregunta de ambito no es "que valor tiene hoy la variable", sino:

- en que partes del programa ese nombre existe con sentido
- a que declaracion esta conectado ese uso del nombre

## 2. Ambito lexico

En el ambito lexico:

- el significado de un nombre depende de donde esta escrito en el codigo
- no depende de quien llamo a quien en tiempo de ejecucion

Lenguajes como Python, Haskell y Rust usan ambito lexico.

Esto significa que, al leer el codigo fuente, normalmente uno puede determinar de antemano a que declaracion corresponde cada nombre.

Esa es una gran ventaja para:

- razonamiento
- compilacion
- closures
- mantenimiento del programa

## 3. Ambito dinamico

En el ambito dinamico:

- el significado de un nombre depende de la pila de llamadas en tiempo de ejecucion

No importa solo donde esta escrito el nombre, sino desde que contexto fue invocado.

Python no usa este modelo.

Si un lenguaje tuviera ambito dinamico, una funcion podria interpretar un nombre libre segun quien la llamo, no segun donde fue definida.

Eso puede ser flexible, pero tambien mucho mas dificil de razonar.

## 4. Binding

El **binding** es la asociacion entre un nombre y algo del programa, por ejemplo:

- un valor
- un objeto
- una ubicacion de memoria
- una funcion

En otras palabras, binding responde la pregunta:

```text
cuando digo X, a que cosa concreta me refiero?
```

Ese "algo" puede variar segun el lenguaje y el nivel de analisis.

## 5. Binding dinamico

Cuando se habla de binding dinamico en este tipo de curso, normalmente se alude a que cierta asociacion se resuelve o cambia en tiempo de ejecucion.

Ejemplos posibles:

- despacho dinamico de metodos
- enlazado dinamico de nombres segun ejecucion

No debe confundirse con ambito dinamico.

## 6. Python: ambito y binding

Python tiene:

- ambito lexico
- mucha resolucion en tiempo de ejecucion

Pero a la pregunta clasica:

```text
Python tiene ambito dinamico?
```

la respuesta es:

```text
No
```

Un ejemplo clasico ayuda a verlo:

```python
x = 10

def outer():
    x = 20
    def inner():
        print(x)
    inner()
```

`inner` imprime `20` porque toma el `x` del entorno lexico donde fue definida, no del lugar desde donde eventualmente podria ser llamada.

## 7. Prolog y binding

En Prolog, las variables se ligan por unificacion.

Ejemplo:

```prolog
X = 3.
```

despues de eso, `X` queda ligada a `3`.

Y:

```prolog
X = 3, X = 4.
```

falla porque la variable ya quedo ligada.

Esto muestra una diferencia fuerte con lenguajes imperativos:

- en Prolog, las variables no son "cajitas" que se actualizan libremente
- son variables logicas que se ligan por unificacion

Esa ligadura no se comporta como asignacion destructiva.

## 8. Cada clausula tiene sus propias variables

En Prolog, las variables de una clausula son locales a esa clausula.

El sistema hace renombramiento automatico para evitar choques accidentales entre distintas invocaciones.

## 9. Respuestas directas

### 1. Python tiene ambito dinamico?

```text
No
```

Python usa ambito lexico o estatico.

### 2. Python tiene binding dinamico?

```text
Si, en varios sentidos practicos
```

Por ejemplo:

- los nombres se resuelven en tiempo de ejecucion
- los objetos tienen tipos dinamicos
- hay despacho dinamico de metodos

Si en clase usan una definicion mas restringida de binding dinamico, conviene seguir esa definicion, pero en el sentido habitual de lenguajes dinamicos la respuesta esperable es `si`.

Lo importante en examen es no confundir esta respuesta con la del ambito. Python puede tener resolucion y comportamiento dinamico en ejecucion, pero sigue siendo de ambito lexico.

### 3. C es de tipos estrictos?

```text
No
```

Porque permite muchas coerciones implicitas y conversiones peligrosas.

### 4. C es de tipos estaticos?

```text
Si
```

Los tipos se conocen principalmente en compilacion.

### 5. Haskell tiene binding dinamico?

```text
No
```

Haskell se caracteriza por ambito lexico y tipado estatico fuerte.

Ademas, en Haskell los bindings se entienden normalmente de forma estatica y declarativa: los nombres quedan asociados por la estructura del programa, no por una pila de llamadas dinamica.

## 10. Cuidado con la trampa

No hay que confundir:

- ambito dinamico
- binding dinamico
- tipado dinamico

Son ideas relacionadas con tiempo de ejecucion, pero no significan lo mismo.

## 11. Ideas clave para estudiar

- ambito = donde un nombre es visible
- binding = asociacion entre nombre y entidad
- Python no tiene ambito dinamico
- C es estatico, pero no especialmente estricto
- Haskell no usa binding dinamico en el sentido clasico

## 12. Regla mnemotecnica

Una forma simple de recordarlo es:

- `scope` pregunta "donde se ve?"
- `binding` pregunta "a que esta ligado?"
- `lexico` significa "segun el texto del programa"
- `dinamico` significa "segun la ejecucion"
