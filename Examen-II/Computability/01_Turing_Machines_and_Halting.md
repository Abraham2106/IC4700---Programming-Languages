# Guia de Computabilidad: Maquinas de Turing y Halting

Esta guia resume las ideas principales sobre:

- maquinas de Turing
- decidibilidad
- maquina de Turing universal
- problema de halting
- contradiccion por autorreferencia
- universalidad de lenguajes

El contenido esta basado en el material de `source_for_turing_and_machines.md`.

## 1. Maquinas de Turing y comportamiento

Sea `M` una maquina de Turing.

Al ejecutarse sobre una entrada `w`, pueden pasar tres cosas:

- `M` acepta `w`
- `M` rechaza `w`
- `M` entra en un ciclo infinito

Decimos que `M` **termina** sobre `w` si acepta o rechaza.

## 2. Lenguajes reconocibles y decidibles

La notacion importante es:

- `L(M)` es el conjunto de strings que `M` acepta
- un lenguaje es **reconocible** si alguna TM lo acepta
- un lenguaje es **decidible** si existe una TM que siempre termina y decide correctamente pertenencia

En el material aparece esta distincion:

- `RE`: lenguajes reconocibles
- `R`: lenguajes decidibles

Idea clave:

- en `RE`, cuando la respuesta es "si", puede encontrarse
- cuando la respuesta es "no", la maquina podria quedarse corriendo para siempre

## 3. Maquina de Turing universal

Una maquina de Turing universal `UTM` recibe como entrada un par:

```text
<M, w>
```

donde:

- `M` es una maquina de Turing
- `w` es una entrada para esa maquina

La `UTM` simula a `M` corriendo sobre `w`.

### Comportamiento

- si `M` acepta `w`, la `UTM` acepta `<M, w>`
- si `M` rechaza `w`, la `UTM` rechaza `<M, w>`
- si `M` entra en loop con `w`, la `UTM` tambien entra en loop con `<M, w>`

Esto formaliza la idea de un interprete general o simulador universal.

## 4. ATM

El lenguaje:

```text
ATM = { <M, w> | M acepta w }
```

es el lenguaje aceptado por la maquina universal.

Esto significa:

- `ATM` es reconocible
- basta simular a `M` con `w` y aceptar si `M` acepta

Pero eso no implica que sea decidible.

## 5. El problema de halting

El problema de halting pregunta:

```text
Dada una maquina M y una entrada w, M termina al ejecutarse sobre w?
```

Formalmente:

```text
HALT = { <M, w> | M termina con w }
```

La idea intuitiva es preguntarse si existe un algoritmo general que, antes de correr el programa completo, nos diga si eventualmente va a terminar.

La respuesta es:

- no existe tal algoritmo general

## 6. Hipotesis de una maquina H

Suponga que existe una maquina `H` tal que, dada una codificacion:

```text
<M, C>
```

responde:

- `Si` si `M` leyendo `C` termina
- `No` si `M` leyendo `C` no termina

Es decir, `H` decidiria perfectamente el problema de halting.

La pregunta es: por que eso lleva a contradiccion?

## 7. La idea de la contradiccion

Si `H` existiera, podriamos construir otra maquina `D` que use a `H` como subrutina, pero haga exactamente lo contrario de lo que `H` predice sobre su propia ejecucion.

La construccion conceptual es:

1. `D` recibe una entrada `x`
2. `D` consulta a `H` sobre `<D, x>`
3. si `H` dice que `D` termina con `x`, entonces `D` entra en loop
4. si `H` dice que `D` no termina con `x`, entonces `D` termina

Ahora analizamos que pasa cuando `D` recibe su propia descripcion como entrada.

## 8. La contradiccion formal

Considere `D(D)`.

Hay dos posibilidades:

### Caso 1

`H` dice que `D(D)` termina.

Entonces, por la definicion de `D`, la maquina entra en loop.

Eso contradice a `H`.

### Caso 2

`H` dice que `D(D)` no termina.

Entonces, por la definicion de `D`, la maquina termina.

Eso tambien contradice a `H`.

## 9. Conclusion

En ambos casos aparece una contradiccion.

Por lo tanto:

- la suposicion de que existe `H` es falsa
- no existe una maquina que decida perfectamente halting para todas las maquinas y entradas

Esta es la razon fundamental por la que el problema de halting es indecidible.

## 10. Conexion con autorreferencia

La prueba depende de una idea muy poderosa:

- una maquina puede razonar sobre otra maquina
- incluso sobre una descripcion de si misma

El material fuente lo relaciona con:

- programas autorreferenciales
- quines
- el recursion theorem

La contradiccion nace al mezclar:

- prediccion perfecta
- autorreferencia
- negacion de la prediccion

## 11. HALT y ATM

Del material se desprenden dos resultados importantes:

### `ATM` no es decidible

No existe un decider general para saber si una maquina acepta una entrada.

### `HALT` no es decidible

No existe un decider general para saber si una maquina termina en una entrada.

### Pero `HALT` si es reconocible

Si una maquina realmente termina, podemos simularla y eventualmente ver que termina.

Por eso:

- `HALT` pertenece a `RE`
- pero no pertenece a `R`

## 12. Que significa esto filosoficamente

Una consecuencia muy importante es:

- hay diferencias entre lo que es verdadero y lo que puede decidirse mecanicamente

Para una entrada dada:

- o la maquina termina
- o no termina

Pero no existe un algoritmo universal que siempre pueda decirnos cual de las dos ocurre.

## 13. Universalidad de Turing

Decir que un lenguaje es **Turing universal** significa que tiene suficiente poder expresivo para simular cualquier computacion de una maquina de Turing.

En otras palabras:

- puede expresar cualquier algoritmo computable
- tiene el mismo poder computacional general que una TM

Muchos lenguajes de programacion generales lo son.

## 14. Consecuencia de ser Turing universal

Si un lenguaje es Turing universal, entonces hereda las limitaciones fundamentales de las maquinas de Turing.

Entre ellas:

- existen programas que no terminan
- el halting problem no puede resolverse perfectamente dentro de ese universo general

## 15. Lenguajes que no deben ser Turing universales

No todos los lenguajes deberian buscar Turing universalidad.

En muchos contextos conviene sacrificar expresividad para ganar garantias.

### Tipos de lenguajes que no conviene que sean Turing universales

- lenguajes de consultas sobre bases de datos
- lenguajes de configuracion
- lenguajes de especificacion declarativa
- lenguajes para analisis estatico o verificacion
- sistemas donde la terminacion garantizada es critica

### Por que

Si un lenguaje no es Turing universal, puede ser mas facil garantizar:

- terminacion
- decidibilidad
- analisis automatico
- optimizacion segura

### Ejemplos

- `Datalog`
- muchos sublenguajes de consultas
- ciertos DSL de configuracion o reglas

## 16. Contraste rapido con Prolog

Esto conecta bien con los temas anteriores:

- Prolog es mucho mas expresivo y puede no terminar
- Datalog restringe la expresividad para garantizar terminacion

Es el mismo trade-off general:

- mas poder
- menos garantias

o:

- menos poder
- mas control teorico

## 17. Definicion Formal de una Maquina de Turing

Formalmente, una Maquina de Turing (TM) se define como una 7-tupla:

$$M = (Q, \Sigma, \Gamma, \delta, q_0, q_{accept}, q_{reject})$$

Donde:
1. $Q$ es un conjunto finito de **estados**.
2. $\Sigma$ es el **alfabeto de entrada** (no contiene el simbolo en blanco $\sqcup$).
3. $\Gamma$ es el **alfabeto de la cinta**, donde $\Sigma \subset \Gamma$ y $\sqcup \in \Gamma$.
4. $\delta$ es la **funcion de transicion**: $\delta: Q \times \Gamma \to Q \times \Gamma \times \{L, R\}$ (donde $L$ representa mover a la izquierda y $R$ mover a la derecha).
5. $q_0 \in Q$ es el **estado inicial**.
6. $q_{accept} \in Q$ es el **estado de aceptacion**.
7. $q_{reject} \in Q$ es el **estado de rechazo**, con $q_{reject} \neq q_{accept}$.

---

## 18. Tesis de Church-Turing

La **Tesis de Church-Turing** establece que nuestra nocion intuitiva de algoritmo (o calculabilidad) es exactamente equivalente a lo que puede ser computado por una Maquina de Turing.

- **Implicacion**: Si un problema no puede ser resuelto por una Maquina de Turing, entonces no existe ningun algoritmo (en ningun lenguaje de programacion presente o futuro) que pueda resolverlo.
- **Universalidad**: Cualquier modelo razonable de computacion (lambda calculo, funciones recursivas $\mu$, computadores modernos, etc.) tiene exactamente el mismo poder computacional que una Maquina de Turing.

---

## 19. Jerarquia de Lenguajes (R vs RE)

Visualizacion de las clases de lenguajes segun decidibilidad y reconocibilidad:

```text
+-------------------------------------------------------+
| Lenguajes No Reconocibles (ej: complementario de ATM) |
|   +-----------------------------------------------+   |
|   | Reconocibles / Turing-Recognizable (RE)       |   |
|   | (ej: ATM, HALT)                               |   |
|   |   +---------------------------------------+   |   |
|   |   | Decidibles / Decidable (R)            |   |   |
|   |   | (ej: Lenguajes Regulares, CFLs)       |   |   |
|   |   +---------------------------------------+   |   |
|   +-----------------------------------------------+   |
+-------------------------------------------------------+
```

### Tabla de Comportamiento:
| Clase de Lenguaje | Entrada pertenece al Lenguaje ($w \in L$) | Entrada NO pertenece al Lenguaje ($w \notin L$) |
| :--- | :--- | :--- |
| **Decidible (R)** | La TM siempre se detiene en $q_{accept}$. | La TM siempre se detiene en $q_{reject}$. |
| **Reconocible (RE)** | La TM eventualmente se detiene en $q_{accept}$. | La TM puede detenerse en $q_{reject}$ o entrar en un ciclo infinito. |

---

## 20. Probar Indecidibilidad mediante Reducciones

Para demostrar que un nuevo problema $B$ es indecidible:
1. Tomamos un problema $A$ que ya sabemos que es indecidible (como $A_{TM}$ o $HALT_{TM}$).
2. Suponemos por contradiccion que existe un decisor $M_B$ para el problema $B$.
3. Construimos un decisor $M_A$ para el problema $A$ que utiliza a $M_B$ como subrutina.
4. Como sabemos que $A$ es indecidible, la existencia de $M_A$ es una contradiccion.
5. Por lo tanto, el decisor $M_B$ no puede existir y el problema $B$ es indecidible.

Esta relacion de reduccion se escribe como:

$$A \le_m B \quad (\text{"A se reduce por mapeo a B"})$$

**Regla de Oro de las Reducciones**:
- Si $A \le_m B$ y $A$ es indecidible $\implies B$ es indecidible.
- Si $A \le_m B$ y $B$ es decidible $\implies A$ es decidible.

---

## 21. Ideas clave para estudiar

- una TM puede aceptar, rechazar o entrar en loop
- `RE` (Turing-reconocible) y `R` (Turing-decidible) no son lo mismo. En `R` la maquina siempre termina.
- la maquina universal (`UTM`) simula a otras maquinas de Turing recibiendo su descripcion y entrada.
- `ATM` es reconocible pero no decidible.
- `HALT` es reconocible pero no decidible.
- la contradiccion surge de autorreferencia mas negacion (diagonalizacion).
- Turing universal significa poder simular cualquier TM.
- algunos lenguajes no deben ser Turing universales si se buscan garantias fuertes (ej: Datalog, SQL, regex).
- **Reduccion**: Es usar una solucion de un problema $B$ para resolver $A$. Si $A$ es imposible y $A$ se reduce a $B$, entonces $B$ es imposible.
- **Tesis de Church-Turing**: El limite de lo computable de forma algoritmica coincide con lo que puede resolver una maquina de Turing.
