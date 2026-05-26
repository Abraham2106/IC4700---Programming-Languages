# Guia de Tipado Fuerte, Debil, Estatico y Dinamico

Esta guia organiza los conceptos de tipado para evitar una confusion muy comun: fuerte/debil no es lo mismo que estatico/dinamico.

## 1. Dos ejes distintos

Hay dos preguntas distintas:

### Cuando se controlan los tipos

- `estatico`: principalmente en compilacion
- `dinamico`: principalmente en ejecucion

### Que tan permisivo es el sistema

- `fuerte`: no deja mezclar tipos incompatibles facilmente
- `debil`: permite coerciones implicitas o usos peligrosos

Esta separacion es importante porque mucha gente mezcla ambas clasificaciones como si fueran una sola.

No lo son.

Por ejemplo:

- Python es dinamico pero fuerte
- C es estatico pero relativamente debil

Entonces "dinamico" no implica "debil", y "estatico" no implica "fuerte".

## 2. Tipado fuerte

En un lenguaje fuertemente tipado:

- operaciones incompatibles producen error
- el lenguaje evita coerciones silenciosas raras

Ejemplo:

```python
"5" + 3
```

en Python produce error, no una conversion sorprendente.

La idea de fondo es que el lenguaje protege la diferencia semantica entre tipos.

Una cadena y un entero no se mezclan solo porque "parece conveniente". Si el programador quiere una conversion, debe hacerla de forma explicita.

Eso suele producir programas mas predecibles, porque las fronteras entre tipos quedan mas claras y las conversiones importantes no ocurren silenciosamente.

## 3. Tipado debil

En un lenguaje debilmente tipado:

- hay coerciones implicitas
- ciertas operaciones mezclan tipos de manera permisiva

Ejemplo en JavaScript:

```javascript
"5" + 3
```

produce:

```text
"53"
```

Y eso no es solo una curiosidad sintactica: muestra que el lenguaje decide coerciones por usted.

Ese comportamiento puede ser util en scripts cortos, pero tambien puede introducir errores sutiles cuando el programador esperaba otra cosa.

Otro ejemplo muy citado es:

```javascript
"5" - 3
```

que produce `2`, mientras que:

```javascript
"5" + 3
```

produce `"53"`.

Ese contraste muestra que la coercion implicita no siempre sigue una intuicion uniforme.

## 4. Tipado estatico

En un lenguaje estatico:

- los tipos se conocen en gran medida antes de ejecutar
- el compilador puede detectar muchos errores

Eso no significa que todos los errores posibles se detecten en compilacion, pero si que una parte importante de la disciplina de tipos ocurre antes de ejecutar el programa.

En la practica, eso ayuda mucho para refactorizar y para mover errores hacia etapas mas tempranas del desarrollo.

Ejemplos:

- C
- Rust
- Haskell

## 5. Tipado dinamico

En un lenguaje dinamico:

- los tipos reales de los valores se manejan en ejecucion

En estos lenguajes, una variable puede referirse a distintos tipos de valores en distintos momentos, siempre que el lenguaje lo permita.

La ventaja es flexibilidad. El costo es que algunos errores aparecen tarde, solo cuando cierta ruta del programa realmente se ejecuta.

Ejemplos:

- Python
- JavaScript

## 6. Casos importantes

### Python

- dinamico
- fuerte

### JavaScript

- dinamico
- debil

### Rust

- estatico
- fuerte

### C

- estatico
- relativamente debil

## 7. C y los tipos estrictos

Si la pregunta es:

```text
C es de tipos estrictos?
```

la respuesta usual es:

```text
No
```

porque:

- permite conversiones implicitas
- permite cast peligrosos
- deja mezclar punteros y enteros en varios contextos

Por eso, aunque C tiene declaraciones de tipos y verificacion en compilacion, no ofrece la misma disciplina fuerte que lenguajes como Haskell o Rust.

Por eso muchas preguntas de examen distinguen entre "estatico" y "estricto" o "fuerte": C cumple mas claramente lo primero que lo segundo.

## 8. Ejemplos utiles

### Python

```python
"5" + 3
```

falla.

### JavaScript

```javascript
"5" + 3
```

produce `"53"`.

### Rust

```rust
let x: i32 = 5;
let y: f64 = x as f64;
```

requiere conversion explicita.

### C

```c
int x = 3.7;
```

trunca silenciosamente.

Dicho de forma mas clara: el lenguaje acepta una conversion potencialmente problematica sin obligar al programador a hacerla explicita.

Ese detalle de "silenciosamente" es justamente parte de la intuicion de debilidad: el lenguaje acepta operaciones potencialmente problemáticas sin obligar al programador a reconocer la conversion de forma explicita.

## 9. Resumen de examen

- Python: dinamico y fuerte
- JavaScript: dinamico y debil
- Rust: estatico y fuerte
- C: estatico y relativamente debil

## 10. Idea clave para estudiar

No mezcle:

- `estatico/dinamico`
- `fuerte/debil`

Un lenguaje puede ser:

- dinamico y fuerte, como Python
- estatico y debil, como C

## 11. Mini tabla mental

Una tabla util para recordar es esta:

```text
Python      -> dinamico, fuerte
JavaScript  -> dinamico, debil
Rust        -> estatico, fuerte
C           -> estatico, relativamente debil
```

## 12. Conexion con errores reales

Estas clasificaciones no son solo teoria.

Afectan cosas muy concretas:

- que errores aparecen temprano
- que conversiones ocurren sin avisar
- que tan facil es refactorizar codigo
- que tan confiable es el analisis del compilador

## 13. Regla corta para examen

Si la pregunta aparece muy resumida, una respuesta util es:

- Python: dinamico y fuerte
- JavaScript: dinamico y debil
- Rust: estatico y fuerte
- C: estatico y relativamente debil

Pero la mejor respuesta no se queda en la tabla: da un ejemplo que muestre por que.
