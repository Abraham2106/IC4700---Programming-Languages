# Prolog (Seguimiento) — Índice Temático Completo

El último núcleo de la evaluación se centra en la Programación Lógica. Exige abandonar el razonamiento imperativo secuencial (asignación de variables y bucles) para adoptar la modelización algorítmica mediante inducción, retroceso y evaluación top-down en el árbol de deducción de primer orden. 

La información se ha estructurado en tres componentes para cubrir minuciosamente los motores internos, las estructuras de datos y los límites teóricos del lenguaje.

---

## Módulos de Estudio

### [1. Unificación, Deducción y Backtracking](/Examen-III/04_A_Unificacion_y_Backtracking.md)
Detalla los cimientos matemáticos y las transigencias (trade-offs) físicas del motor de Prolog.
- Deducción lógica estructurada mediante generalización del **Modus Ponens Universal**.
- Reglas de simplificación del algoritmo de **Unificación de Martelli-Montanari**.
- El límite computacional del **Occurs Check** y el colapso de las estructuras cíclicas.
- Búsqueda en el Árbol SLD con **Depth-First Search (DFS)**, el problema de la recursividad por la izquierda y el manejo invasivo del espacio de control con el **Operador Corte (!)**.

### [2. Estructuras Recursivas y Problemas CSP](/Examen-III/04_B_Listas_y_CSP.md)
Cubre el desensamblaje lógico de datos compuestos y la formulación avanzada del control de dominio.
- El modelo cabeza-cola `[H|T]` y su explotación algorítmica multidireccional con `append/3`.
- Optimización asintótica: Inversión ingenua cuadrática contra la Inversión Lineal con **Acumuladores (Tail Recursion / TCO)**.
- Recorridos matriciales implícitos usando bucles lógicos recursivos.
- Problemas de Satisfacción de Restricciones (CSP) evidenciando el desastroso anti-patrón **Generate-and-Test** contra la optimización agresiva de **Constrain-and-Generate** (Poda temporal de ramas).

### [3. Datalog: Expresividad contra Garantías Teóricas](/Examen-III/04_C_Datalog.md)
Presenta a Datalog, la contracara matemática a las vulnerabilidades de la aserción de estados de Prolog.
- Datalog asegura el escape al *Halting Problem* castrando el lenguaje e imposibilitando la generación infinita (**Functores de aridad cero** y omisión de Listas).
- División modular estricta de la información en Base de Datos Extensional (**EDB**) e Intensional (**IDB**).
- La **Regla de Seguridad en Variables**: obligación lógica del anclaje positivo a un universo material finito.
- El colapso cíclico por negación y cómo Datalog lo neutraliza forzando **Estratificación** deductiva.
