# Guia de Sistemas Distribuidos Confiables: Erlang OTP

Esta guia resume los conceptos principales de confiabilidad y tolerancia a fallos asociados al modelo Erlang/OTP.

Temas cubiertos:

- confiabilidad en sistemas distribuidos
- fail-fast
- process pairs
- supervision trees
- estrategias de reinicio
- MTBF
- tipos de bugs relevantes
- condiciones para mejorar la confiabilidad con procesos concurrentes

## 1. Idea general

En sistemas distribuidos confiables, el objetivo no es evitar que algo falle alguna vez.

El objetivo real es:

- aislar los fallos
- detectarlos rapido
- recuperarse automaticamente
- mantener el servicio disponible

Erlang/OTP fue diseñado justamente con esa filosofia.

## 2. Fail-fast

El principio de `fail-fast` dice:

si un componente detecta un estado invalido o inesperado, debe fallar inmediatamente y de forma visible.

### Por que esto es bueno

Un fallo temprano y ruidoso suele ser mejor que:

- continuar ejecutando con datos corruptos
- esconder el problema
- producir errores mas graves despues

### Idea central

Es mas facil reiniciar un proceso roto que diagnosticar un sistema que sigue vivo pero corrupto.

## 3. Process pairs

Los `process pairs` son una tecnica de redundancia.

La idea es:

- dos procesos ejecutan la misma tarea en paralelo
- uno actua como primario
- el otro actua como secundario o respaldo

Si el primario falla:

- el secundario toma el control
- el servicio continua sin interrupcion fuerte

### Requisito importante

Ambos procesos deben mantener sincronizado el estado necesario para que la transicion sea transparente.

## 4. Supervision trees

Los `supervision trees` son una estructura jerarquica de procesos de OTP.

En esta estructura:

- los supervisores monitorean a sus procesos hijos
- los hijos suelen ser workers o incluso otros supervisores

Cuando un hijo falla:

- el supervisor detecta el fallo
- decide como reaccionar
- reinicia uno o varios procesos segun la estrategia definida

## 5. Estrategias de reinicio

### `one_for_one`

Si un hijo falla:

- solo se reinicia ese hijo

### `one_for_all`

Si un hijo falla:

- se reinician todos los hijos del supervisor

### `rest_for_one`

Si un hijo falla:

- se reinicia el fallado
- tambien se reinician los hijos que dependen de el en el orden correspondiente

## 6. Cuando usar cada estrategia

### `one_for_one`

Conviene cuando:

- los workers son bastante independientes
- el fallo de uno no corrompe a los otros

### `one_for_all`

Conviene cuando:

- los procesos comparten una inicializacion comun
- si uno falla, los demas pueden quedar en estado inconsistente

### `rest_for_one`

Conviene cuando:

- existe dependencia entre procesos segun orden de arranque
- los procesos posteriores dependen del estado de uno anterior

## 7. MTBF

`MTBF` significa:

```text
Mean Time Between Failures
```

Es el tiempo promedio entre dos fallos consecutivos de un sistema.

### Interpretacion

- a mayor MTBF, mayor confiabilidad
- un sistema con MTBF alto falla con menos frecuencia

Es una de las metricas centrales en sistemas confiables.

## 8. Relacion entre MTBF y disponibilidad

Una formula conceptual comun es:

```text
Disponibilidad = MTBF / (MTBF + MTTR)
```

donde:

- `MTBF` es tiempo medio entre fallos
- `MTTR` es tiempo medio de reparacion

### Idea importante

La disponibilidad mejora:

- si los fallos ocurren menos seguido
- o si el sistema se recupera mas rapido

OTP ayuda especialmente a reducir el tiempo de recuperacion con supervision y reinicio automatico.

## 9. Heisenbugs

Los `Heisenbugs` son bugs dificiles de reproducir cuyo comportamiento cambia o desaparece al intentar observarlos.

Ejemplos tipicos:

- bugs de concurrencia
- race conditions
- problemas sensibles al timing

### Por que son peligrosos

Porque:

- no aparecen siempre
- agregar logs puede cambiarlos
- usar un debugger puede ocultarlos

## 10. Sobre "Eigenbugs"

El termino `Eigenbugs` no es estandar en la literatura clasica.

Es posible que el curso se refiera a:

- `Bohrbugs`, que son reproducibles y deterministas
- o a una denominacion propia del curso

### Recomendacion

Conviene verificar con el material oficial del curso o con lo visto en clase si el examen usa una definicion especifica para `Eigenbugs`.

## 11. Bohrbugs, Heisenbugs y otros tipos

Para ubicar el concepto, esta clasificacion suele ser util:

- `Bohrbugs`: reproducibles, deterministas, como un bug logico clasico
- `Heisenbugs`: desaparecen o cambian al observarlos
- `Mandelbugs`: comportamiento muy caotico y sensible al contexto
- `Schroedinbugs`: existen latentes hasta que algo los activa

## 12. Por que OTP ayuda con la confiabilidad

El modelo OTP mejora la confiabilidad porque:

- separa los componentes en procesos aislados
- un fallo local no tiene por que derribar todo
- los supervisores monitorean y reinician
- se favorece el estado minimo y recuperable
- se aplica el principio fail-fast

## 13. Condiciones para que dividir en procesos mejore la confiabilidad

No basta con partir un sistema en muchos procesos. Para que la descomposicion realmente aumente la confiabilidad, se necesitan varias condiciones.

### Aislamiento de fallos

El fallo de un proceso no debe propagarse facilmente a otros.

Esto mejora mucho cuando:

- no hay memoria compartida
- la comunicacion es por mensajes

### Redundancia

Los componentes criticos deben tener replicas o pares de respaldo.

### Supervision y reinicio automatico

Debe existir un mecanismo que:

- detecte el fallo
- reinicie rapido
- restaure el servicio

### Independencia

Los procesos no deben formar dependencias ciclicas fragiles que hagan que el fallo de uno derrumbe a todos.

### Estado minimo y recuperable

Cada proceso debe mantener solo el estado necesario y poder reconstruirlo al reiniciarse.

## 14. Contraste con memoria compartida

En modelos de memoria compartida:

- varios hilos pueden tocar el mismo estado
- aparecen races, deadlocks y corrupcion de datos

En el modelo de Erlang:

- los procesos estan aislados
- no comparten memoria mutable
- se comunican por paso de mensajes

Esto simplifica mucho el manejo de fallos.

## 15. Ejemplo conceptual de arbol de supervision

Imagine esta jerarquia:

```text
Supervisor principal
|- Supervisor de pagos
|  `- Worker de pagos
|- Supervisor de inventario
|  `- Worker de inventario
`- Supervisor de notificaciones
   `- Worker de notificaciones
```

Si falla el worker de notificaciones:

- su supervisor puede reiniciarlo
- el sistema de pagos puede seguir funcionando

Esto ilustra el aislamiento de fallos.

## 16. Caso de diseno: e-commerce

Suponga un sistema con:

- procesador de pagos
- gestor de inventario
- servicio de notificaciones

### Analisis

El procesador de pagos es critico.

Si falla:

- puede afectar el flujo de compra
- el sistema debe recuperarlo rapido

Las notificaciones son menos criticas.

Si fallan:

- las compras aun pueden completarse

### Una jerarquia razonable

```text
Supervisor raiz
|- Supervisor critico
|  |- Worker de pagos
|  `- Worker de inventario
`- Supervisor auxiliar
   `- Worker de notificaciones
```

### Posible estrategia

- `one_for_all` o `rest_for_one` en la parte critica si pagos e inventario deben mantenerse consistentes
- `one_for_one` para notificaciones, ya que su fallo no deberia tumbar al resto

## 17. Disponibilidad y recuperacion

La confiabilidad no depende solo de que los procesos fallen poco.

Tambien depende de que:

- fallen de forma controlada
- se detecte rapido el fallo
- la reparacion automatica sea veloz

Por eso OTP se centra en:

- supervision
- reinicio
- aislamiento

## 18. Ideas clave para estudiar

- fail-fast significa fallar rapido y visiblemente
- los process pairs aportan redundancia
- los supervision trees estructuran la recuperacion
- `one_for_one`, `one_for_all` y `rest_for_one` tienen usos distintos
- MTBF mide el tiempo promedio entre fallos
- reducir MTTR mejora disponibilidad
- los Heisenbugs son especialmente peligrosos en concurrencia
- OTP mejora la confiabilidad al aislar procesos y reiniciarlos automaticamente
