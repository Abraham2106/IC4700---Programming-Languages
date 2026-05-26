# Guia de Confiabilidad y Sistemas Distribuidos

Esta guia responde las preguntas conceptuales sobre confiabilidad, fallos y organizacion de procesos en sistemas distribuidos.

Temas cubiertos:

- pares de procesos
- arboles de supervision
- fail-fast
- Heisenbugs y Eigenbugs
- MTBF
- condiciones para mejorar confiabilidad en sistemas concurrentes

## 1. Pares de procesos

Los **pares de procesos** son una tecnica de redundancia.

La idea es:

- dos procesos ejecutan la misma tarea en paralelo
- uno actua como principal
- otro actua como respaldo

Si el proceso principal falla:

- el secundario toma el control
- el servicio puede continuar con poca o ninguna interrupcion

### Requisito importante

Para que esto funcione bien, ambos procesos deben mantener sincronizado el estado necesario.

Si no comparten suficiente estado:

- el relevo no sera transparente

## 2. Arboles de supervision

Los **arboles de supervision** son estructuras jerarquicas de procesos donde unos procesos especiales, llamados supervisores, observan a otros procesos hijos.

Cuando un hijo falla:

- el supervisor detecta el fallo
- aplica una politica de recuperacion
- normalmente reinicia el proceso

### Estrategias comunes

- `one_for_one`: reinicia solo el hijo que fallo
- `one_for_all`: reinicia todos los hijos del supervisor
- `rest_for_one`: reinicia el fallado y los que dependen de el

## 3. Fail-fast

El principio de **fail-fast** dice que si un componente detecta un estado invalido o inesperado, debe fallar inmediatamente y de forma visible.

### Por que es util

Es preferible:

- fallar pronto
- detectar el problema facil

que:

- seguir ejecutandose con estado corrupto
- esconder el error
- dañar datos o propagar inconsistencias

En sistemas confiables, muchas veces es mejor reiniciar limpio que continuar mal.

## 4. Eigenbugs y Heisenbugs

Estos terminos se usan para describir fallos dificiles de analizar en sistemas reales.

### Heisenbugs

Los **Heisenbugs** son bugs que cambian o desaparecen cuando uno intenta observarlos.

La alusion viene del principio de incertidumbre de Heisenberg: observar el sistema altera el fenomeno que se quiere estudiar.

Ejemplos tipicos:

- bugs de concurrencia
- bugs sensibles al timing
- carreras entre procesos o hilos
- corrupciones de memoria que dependen del estado global del sistema

Son peligrosos porque:

- no siempre aparecen
- agregar logs puede alterarlos
- el debugger puede hacer que desaparezcan
- muchas veces dependen de interacciones raras que no se reproducen facilmente

### Eigenbugs

En el contexto de estos apuntes, los **Eigenbugs** son pulgas o fallos que sobreviven a un proceso disciplinado de pruebas y bugtracking, y que terminan manifestandose en el producto entregado.

La hipotesis es que aparecen por condiciones globales del estado de memoria o del sistema que solo se dan fortuitamente despues de cierto tiempo de ejecucion.

Ejemplo conceptual:

- un buffer se excede
- escribe accidentalmente un byte en otra zona de memoria
- se corrompe una variable del entorno que rara vez se usa
- mas adelante un programa depende de esa variable
- desde ahi el sistema empieza a comportarse de forma erratica

### Idea comun

Tanto los Heisenbugs como los Eigenbugs son dificiles de reproducir y diagnosticar porque no suelen depender de un error local, simple y determinista, sino de estados complejos, timing o corrupciones que aparecen solo en circunstancias especiales.

## 6. MTBF

`MTBF` significa:

```text
Mean Time Between Failures
```

Es el tiempo promedio entre fallos consecutivos de un sistema.

### Interpretacion

- a mayor `MTBF`, mayor confiabilidad
- si un sistema falla menos frecuentemente, su tiempo medio entre fallos aumenta

## 7. Cuando dividir en procesos mejora la confiabilidad

No basta con partir un sistema grande en muchos procesos. Para que el sistema resultante sea mas confiable y aumente su `MTBF`, deben cumplirse ciertas condiciones.

## 8. Aislamiento de fallos

El fallo de un proceso no debe propagarse automaticamente a los demas.

Esto mejora cuando:

- no hay memoria compartida peligrosa
- la comunicacion ocurre por mensajes
- cada proceso esta encapsulado

## 9. Redundancia

Los componentes criticos deben tener respaldo.

Ejemplos:

- replicas
- pares de procesos
- procesos reiniciables

Sin redundancia, un fallo aislado puede seguir siendo fatal para el sistema completo.

## 10. Supervision y reinicio automatico

Debe existir un mecanismo que:

- detecte el fallo
- reinicie el componente
- recupere el servicio rapido

Esto no necesariamente evita el fallo, pero reduce mucho el impacto.

## 11. Independencia estructural

Los procesos no deben tener dependencias ciclicas fragiles que hagan que un fallo local derrumbe a todo el sistema.

Si un proceso depende de muchos otros de manera estrecha:

- el sistema pierde modularidad
- los fallos pueden propagarse con facilidad

## 12. Estado minimo y recuperable

Cada proceso deberia mantener solo el estado necesario y, preferiblemente, ese estado deberia poder reconstruirse.

Esto facilita:

- reinicio limpio
- recuperacion
- reemplazo por una replica

## 13. Relacion con disponibilidad

La confiabilidad observada no depende solo de cuan seguido hay fallos.

Tambien depende de:

- cuan rapido se detectan
- cuan rapido se reparan
- cuan bien se contiene su propagacion

Por eso, en la practica, aumentar disponibilidad implica combinar:

- `MTBF` alto
- `MTTR` bajo

## 14. Respuestas cortas para examen

### Que son pares de procesos

Dos procesos redundantes que ejecutan la misma tarea para que uno pueda reemplazar al otro si falla.

### Que son arboles de supervision

Jerarquias de procesos donde supervisores monitorean hijos y los reinician segun politicas definidas.

### Que es fail-fast

Principio de fallar de inmediato y visiblemente ante un estado invalido, en vez de continuar corruptamente.

### Que son Heisenbugs

Bugs no reproducibles o muy sensibles a la observacion, comunmente asociados a concurrencia y timing.

### Que son Eigenbugs

Fallos que sobreviven las pruebas normales y aparecen por condiciones globales del estado del sistema o de la memoria, a menudo tras cierto tiempo de ejecucion y con comportamiento erratico.

### Que significa MTBF

`Mean Time Between Failures`, tiempo promedio entre fallos.

### Que condiciones aumentan confiabilidad

- aislamiento de fallos
- redundancia
- supervision y reinicio automatico
- independencia entre procesos
- estado minimo y recuperable
