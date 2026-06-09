# Prolog — Unificación, Deducción y Backtracking

Prolog (Programming in Logic) elimina la distinción entre especificar un problema y programar su solución. En lugar de escribir una secuencia de instrucciones mutables, el programador declara hechos y reglas lógicas, y un motor de inferencia subyacente deduce las respuestas mediante un algoritmo de resolución de primer orden.

---

## 1. Deducción Lógica y el Modus Ponens Universal

Prolog se basa en un subconjunto de la lógica de primer orden conocido como Cláusulas de Horn. El programa se estructura matemáticamente en Cláusulas:
$$ A_0 \leftarrow A_1, ..., A_n $$
Donde $A_0$ es la cabeza (la conclusión) y $A_1 ... A_n$ es el cuerpo (las premisas). Si el cuerpo está vacío, la cláusula se denomina **Hecho**. Si tiene cuerpo, es una **Regla**.

### 1.1 El Motor de Inferencia

Cuando se introduce una consulta (Query) $Q$, Prolog utiliza una generalización de la regla de inferencia del Modus Ponens:
- Si existe una regla en el programa cuya cabeza coincida estructuralmente con la consulta $Q$.
- Y si el motor logra deducir satisfactoriamente todas y cada una de las metas en el cuerpo de esa regla (las submetas $A_1 ... A_n$).
- Entonces, $Q$ es una Consecuencia Lógica válida del programa (es decir, es verdadera).

Este proceso se realiza de forma top-down recursiva, descomponiendo metas complejas en submetas cada vez más pequeñas hasta llegar a los hechos base.

---

## 2. El Algoritmo de Unificación de Martelli-Montanari

El corazón mecánico de Prolog es la **Unificación**. No existen operaciones de "asignación" o "mutación" de variables (`x = 5`). En su lugar, las variables actúan como marcadores lógicos (placeholders) que intentan "encajar" con otros términos hasta que ambos lados de la ecuación sean idénticos estructuralmente.

### 2.1 Reglas de Simplificación

Para unificar dos términos, Prolog aplica iterativamente un conjunto estricto de transformaciones:
1. **Descomposición:** Si los functores principales y la aridad coinciden, se descomponen. Ej: unificar `padre(X, b)` con `padre(a, b)` requiere unificar `X` con `a` y `b` con `b`. Si los functores o aridades difieren, hay **Conflicto** y la unificación falla inmediatamente.
2. **Borrado:** Identidades como `b = b` se eliminan.
3. **Intercambio:** Ecuaciones `a = X` se giran lógicamente a `X = a`.

La sustitución final (ej. $\sigma = \{X \mapsto a\}$) se conoce como la **Unificadora Más General (MGU)**, porque es la sustitución mínima y menos restrictiva posible para lograr la igualdad.

### 2.2 El Problema del Occurs Check

Teóricamente, una variable no puede unificarse con un término recursivo que la contenga (ej. `X = f(X)`), ya que esto implicaría un árbol estructural de tamaño infinito. Una unificación matemáticamente pura requiere un paso de eliminación con **Occurs Check** (Verificación de Ocurrencia) para abortar si $X$ aparece dentro del término derecho.

Sin embargo, recorrer árboles para buscar variables cuesta tiempo ($O(n)$). El Prolog clásico omite intencionalmente esta revisión por motivos de rendimiento, lo que permite que el programador cree inadvertidamente estructuras cíclicas en memoria que pueden desembocar en un desbordamiento de pila catastrófico (Stack Overflow).

---

## 3. Resolución Procedimental: DFS y Backtracking

El significado lógico declarativo de Prolog choca frontalmente con su implementación computacional, demostrando que Prolog **no es un demostrador de teoremas perfecto**, sino un lenguaje de programación procedimental disfrazado.

### 3.1 Depth-First Search y el Árbol SLD

Cuando Prolog enfrenta una consulta que tiene múltiples reglas posibles que la satisfacen (un Choice Point), no puede evaluarlas todas en paralelo.
En su lugar, sigue un orden **Depth-First Search (DFS)** estricto:
1. Evalúa las cláusulas en el **orden físico exacto de escritura** en el archivo.
2. Explora las submetas del cuerpo de **izquierda a derecha**.
3. Si alcanza un Hecho verdadero, reporta la solución de esa rama del árbol de resolución (SLD Tree).
4. Si encuentra una contradicción o se solicita otra solución, invoca el **Backtracking**: retrocede al último Choice Point, deshace y destruye cualquier unificación de variables ligada en la rama fallida, y desciende por la siguiente regla alternativa disponible.

### 3.2 Incompletitud por Ramas Infinitas y Recursión por la Izquierda

El DFS requiere muy poca RAM porque solo debe mantener una rama activa en memoria ($O(\text{profundidad})$), a diferencia de BFS que requeriría memoria exponencial.
Sin embargo, el costo arquitectónico de DFS es la **Incompletitud Matemática**.

Considere una regla lógicamente cierta, pero recursiva por la izquierda:
```prolog
camino(X, Z) :- camino(Y, Z), arista(X, Y).
```
Debido a la evaluación DFS izquierda-derecha, Prolog evaluará `camino(Y, Z)` invocando de nuevo `camino(_, Z)`, cayendo en un ciclo recursivo ciego e infinito. Prolog quedará atrapado en esta rama infinitamente profunda y jamás encontrará las soluciones válidas que pudieran estar alojadas en ramas paralelas adyacentes del árbol de resolución.

---

## 4. El Control de Flujo Físico: El Operador Corte (!)

Para aliviar la búsqueda ineficiente del DFS, Prolog introdujo el operador Corte (`!`), una herramienta imperativa que destruye nodos en el árbol de resolución alterando la pureza lógica del lenguaje.

El corte siempre se evalúa como `true`, pero su efecto colateral primario es **bloquear el backtracking**. Cualquier Choice Point que haya sido creado desde que se invocó la regla actual queda aniquilado; Prolog no podrá retroceder más allá del corte para buscar alternativas.

- **Corte Verde (Green Cut):** Poda ramas del árbol que el programador *sabe matemáticamente* que no contienen soluciones válidas. No altera la lógica declarativa del programa, solo aumenta su velocidad reduciendo el espacio de búsqueda.
- **Corte Rojo (Red Cut):** Depende asimétricamente del orden de evaluación. Si se elimina el corte rojo, el programa devuelve resultados lógicamente erróneos. Implica un fuerte acoplamiento procedimental y es considerado un anti-patrón de diseño en la programación lógica purista.
