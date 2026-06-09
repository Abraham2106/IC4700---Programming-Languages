# Semántica Operacional — Revisión de Tipos (Type Checking)

La validación estática garantiza que un programa tenga sentido antes de ser ejecutado. La revisión de tipos en el diseño de compiladores se formaliza a través de un sistema de tipos matemático que dicta reglas para la validación (checking) y la inferencia (inference).

---

## 1. El Sistema de Tipos Formal

El análisis semántico se independiza del lenguaje de implementación (C++, Java, Haskell) mediante reglas de inferencia lógicas.

### 1.1 Reglas de Inferencia y Juicios (Judgements)

Una regla de inferencia consiste en un conjunto de premisas $J_1, ..., J_n$ y una conclusión $J$, separadas por una línea horizontal. La forma más común de juicio tipográfico es $e : T$, que se lee como *"la expresión $e$ tiene el tipo $T$"*.

Para evaluar variables se requiere un contexto o tabla de símbolos. El contexto, denotado por $\Gamma$ (Gamma), almacena los pares (variable, tipo). Así, la notación completa es:
$$\Gamma \vdash e : T$$
*(Se lee: "En el contexto $\Gamma$, la expresión $e$ tiene tipo $T$").*

### 1.2 Inferencia vs Comprobación

- **Type Checking (Comprobación):** Dada una expresión $e$ y un tipo esperado $T$, decidir si es cierto que $e : T$. Las reglas de comprobación devuelven silencio si son exitosas o un error si fallan.
- **Type Inference (Inferencia):** Dada una expresión $e$, calcular y devolver un tipo $T$ tal que $e : T$.

Por ejemplo, para validar $a \ \&\&\ b$, la inferencia comprueba lógicamente que tanto $a$ como $b$ sean booleanos y luego retorna el tipo final:
```
infer(a && b):
  check(a, bool)
  check(b, bool)
  return bool
```

---

## 2. Contextos y Ámbito de Variables

Un contexto no es un simple diccionario plano; los lenguajes con estructura de bloques (como C o Java) exigen reglas de visibilidad precisas.

### 2.1 La Pila de Contextos

Para manejar el ocultamiento de variables (shadowing) introducido por bloques de llaves `{ }`, $\Gamma$ se implementa como una pila de tablas de búsqueda. 
- Una variable declarada dentro de un bloque extiende el contexto en la cima de la pila.
- Al salir del bloque, se desapila la tabla superior, descartando todas las variables locales.
- Al buscar una variable, el compilador busca desde la cima (el bloque más interno) hacia la base (el entorno global).

### 2.2 Validación de Bloques y Funciones

Una sentencia (statement) no tiene un tipo evaluable, sino que posee un estado de "validez": $\Gamma \vdash s \text{ valid}$.

La regla de declaración de variables altera el contexto para las sentencias subsecuentes:
$$\frac{\Gamma, x:T \vdash s_2...s_n \text{ valid}}{\Gamma \vdash T\ x; \ s_2...s_n \text{ valid}}$$

En el caso de las funciones, los parámetros actúan como el entorno base para el cuerpo de la función. El chequeo general del programa ocurre en dos pasadas (Two passes): primero se recolectan las firmas de todas las funciones (permitiendo recursión mutua), y luego se comprueba el cuerpo de cada una.

---

## 3. Sobrecarga y Conversión de Tipos

Cuando un operador como `+` acepta múltiples tipos de datos (int, double, string), el compilador debe lidiar con la sobrecarga sintáctica y las conversiones de hardware.

### 3.1 Despacho de Sobrecarga

Para tipos primitivos sobrecargados, se infiere el tipo del primer operando y se exige el mismo para el segundo.
```
infer(a + b):
  t = infer(a)
  check(b, t)
  return t
```

### 3.2 Conversiones Explícitas (Type Coercion)

Si los tipos no coinciden, se introducen reglas de conversión (siempre de menor a mayor pérdida de información, ej. `int < double < string`). El compilador inyecta un nodo de conversión oculto en el Árbol de Sintaxis Abstracta (AST) para decirle al generador de código ensamblador que utilice instrucciones de hardware de conversión (ej. pasar el entero a registro de punto flotante).

---

## 4. Implementación en el Compilador

La traducción dirigida por la sintaxis se implementa mediante patrones de diseño específicos según el lenguaje hospedador.

### 4.1 Árboles Anotados

El resultado del Type Checker no es solo un "OK", sino un nuevo AST donde cada nodo tiene anotado el tipo que le fue inferido. Por ejemplo, en lugar de retornar `a + b`, el chequeador retorna `[a' + b' : T]`. Estas anotaciones son vitales para la fase posterior de generación de código máquina.

### 4.2 Enfoque Funcional (Haskell)

Haskell implementa el compilador como una serie de computaciones encadenadas en mónadas. Utiliza `Err` (para propagar errores sin crashear) o `IO` (para el estado del compilador). El Pattern Matching puro recorre el AST delegando constructores de forma natural:
```haskell
inferExp env (EAdd exp1 exp2) = do
    typ <- inferExp env exp1
    checkExp env exp2 typ
    return typ
```

### 4.3 Enfoque Orientado a Objetos (Java: Visitor Pattern)

Dado que Java carece de pattern matching avanzado, usa el Patrón Visitante (`Visitor<R, A>`) para modularizar las fases del compilador. El chequeador de tipos implementa un visitante donde el argumento `A` es el Contexto (`Env`) y el valor de retorno `R` es el Tipo inferido (`Type`). Cada nodo del AST tiene un método `accept` que desvía la ejecución a la implementación correcta del tipo.
