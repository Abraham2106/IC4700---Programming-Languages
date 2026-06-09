# Semántica Operacional — Índice Temático Completo

El compilador no solo entiende las reglas gramaticales, debe garantizar la cordura física del programa y planificar su ejecución a nivel atómico en la máquina. Este módulo formaliza estas garantías en un marco de trabajo de sistemas de tipos y modelos de ejecución (Big-Step y Small-Step).

Para mantener el rigor de la prosa y no ignorar información crítica, el contenido está distribuido en dos submódulos especializados.

---

## Módulos de Estudio

### [1. Revisión de Tipos (Type Checking)](/Examen-III/03_A_Revision_de_Tipos.md)
Desglosa los juicios formales (Judgements) necesarios para que un lenguaje determine la sanidad de su código antes de emitir ensamblador.
- Inferencia Matemática vs Comprobación Directa.
- El Contexto ($\Gamma$), la Pila de Contextos para el Shadowing de Bloques y validación de funciones.
- Resolviendo colisiones de Sobrecarga y Conversiones Lógicas implícitas.
- Implementación de Type Checkers mediante Mónadas de Error y Árboles Anotados (Haskell) frente a implementaciones OOP con el Patrón Visitante (Java).

### [2. Interpretación y Máquinas Virtuales](/Examen-III/03_B_Interpretacion.md)
Traduce el código válido en operaciones con efectos de estado y administración física pura de la memoria.
- Semántica Operacional (Big-Step) con Evaluación de Entornos mutados ($\gamma(x:=v)$).
- El costo arquitectónico del Paso por Valor frente al corto-circuito (Lazy Evaluation / Call by Need).
- Semántica Small-Step para Máquinas Virtuales (JVM), Punteros de Código (P), la Pila Estática (S) y control de flujo mediante Bytecode (`ifeq`, `goto`).
- Administración agnóstica de Heap dinámico masivo usando algoritmos de recolección de basura (Mark-Sweep).
