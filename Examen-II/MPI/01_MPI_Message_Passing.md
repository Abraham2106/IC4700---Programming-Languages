# Guia de MPI: Programacion Distribuida por Paso de Mensajes

Esta guia resume el modelo de paso de mensajes y la API basica de MPI para resolver problemas pequenos de computacion distribuida.

Temas cubiertos:

- memoria compartida vs paso de mensajes
- procesos, `rank` y `size`
- `MPI_Init` y `MPI_Finalize`
- `MPI_Send` y `MPI_Recv`
- tags y comunicadores
- patron maestro-esclavo
- suma distribuida

## 1. Modelo de paso de mensajes

En MPI, cada proceso tiene su propia memoria privada.

Eso significa que:

- los procesos no comparten variables
- para intercambiar informacion deben enviarse mensajes
- la comunicacion es explicita

### Ventaja

Este modelo evita por diseno muchos problemas de memoria compartida, como las condiciones de carrera sobre una variable global.

### Desventaja

La comunicacion tiene costo. Enviar mensajes por red o entre procesos suele ser mas lento que acceder a memoria local.

## 2. MPI y el comunicador global

En la mayoria de programas introductorios se usa:

```c
MPI_COMM_WORLD
```

Este comunicador incluye a todos los procesos del programa.

Cada proceso tiene:

- un `rank`: su identificador unico
- un `size`: la cantidad total de procesos

## 3. Inicializacion y finalizacion

Todo programa MPI debe iniciar y cerrar el entorno MPI:

```c
MPI_Init(&argc, &argv);
MPI_Finalize();
```

### Punto critico

`MPI_Init` recibe `&argc` y `&argv`. No debe llamarse sin argumentos.

Forma correcta:

```c
int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Finalize();
    return 0;
}
```

## 4. Saber quien soy: `rank` y `size`

```c
int rank, size;

MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);
```

### Significado

- `rank` identifica al proceso actual
- `size` indica cuantos procesos hay en total

Si se ejecutan 4 procesos, los `rank` seran:

```text
0, 1, 2, 3
```

Normalmente:

- `rank == 0` se usa como proceso maestro
- `rank != 0` se usa como proceso trabajador

## 5. Envio y recepcion bloqueante

Las operaciones basicas son:

```c
MPI_Send(buffer, count, datatype, dest, tag, communicator);
MPI_Recv(buffer, count, datatype, source, tag, communicator, &status);
```

### Parametros importantes

- `buffer`: direccion del dato
- `count`: cantidad de elementos
- `datatype`: tipo MPI, por ejemplo `MPI_INT`
- `dest`: proceso destino
- `source`: proceso origen
- `tag`: etiqueta del mensaje
- `communicator`: normalmente `MPI_COMM_WORLD`

## 6. Operaciones bloqueantes

`MPI_Send` y `MPI_Recv` son bloqueantes en este nivel introductorio.

Eso significa:

- el `send` espera hasta completar la operacion
- el `recv` espera hasta que llegue el mensaje correcto

Si el patron de envio y recepcion esta mal diseñado, puede haber espera indefinida o deadlock.

## 7. Importancia del tag

El `tag` sirve para distinguir mensajes.

Por ejemplo:

```c
MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
MPI_Recv(&y, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
```

Ambos usan `tag = 0`, por lo que pueden emparejarse correctamente.

### Punto critico

El `tag` del `Send` debe coincidir con el `tag` esperado por el `Recv`.

## 8. Patron maestro-esclavo

Un patron muy comun es:

- el proceso `0` coordina
- los demas procesos calculan algo
- luego le envian su resultado al proceso `0`

Este patron aparece mucho en ejercicios de sumas distribuidas, minimos, maximos y conteos.

## 9. Programa completado: suma de numeros enviados al maestro

La idea es:

- cada proceso genera un numero
- si el proceso es `0`, recibe todos los numeros y los suma
- si el proceso no es `0`, envia su numero al proceso `0`

Codigo:

```c
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int size, rank;
    int numero;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    numero = rank * 10;

    if (rank == 0) {
        int suma = numero;
        int recibido;

        for (int i = 1; i < size; i++) {
            MPI_Recv(&recibido, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            suma += recibido;
        }

        printf("Suma total: %d\n", suma);
    } else {
        MPI_Send(&numero, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
```

## 10. Explicacion del programa

### Paso 1

Todos los procesos ejecutan el mismo codigo:

```c
MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &size);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
```

### Paso 2

Cada proceso genera un valor:

```c
numero = rank * 10;
```

Por ejemplo, con 4 procesos:

- proceso 0 genera `0`
- proceso 1 genera `10`
- proceso 2 genera `20`
- proceso 3 genera `30`

### Paso 3

Si `rank == 0`, el proceso maestro:

- empieza la suma con su propio numero
- recibe un numero de cada trabajador
- acumula todos los resultados

### Paso 4

Si `rank != 0`, el proceso trabajador:

- envia su valor al maestro con `MPI_Send`

## 11. Puntos criticos de la API

### `MPI_Init`

Debe recibir:

```c
MPI_Init(&argc, &argv);
```

No debe escribirse sin argumentos.

### `MPI_Send` y `MPI_Recv`

Son bloqueantes en este contexto.

Eso significa que el proceso espera hasta que la operacion completa.

### `tag`

El `tag` debe coincidir entre `Send` y `Recv` para que el mensaje se empareje correctamente.

### `MPI_COMM_WORLD`

Es el comunicador global que incluye a todos los procesos.

## 12. Riesgo clasico: deadlock

Si dos procesos hacen `MPI_Send` uno al otro al mismo tiempo antes de hacer `MPI_Recv`, el programa puede quedar bloqueado.

Ejemplo conceptual:

- proceso 0 intenta enviar a proceso 1
- proceso 1 intenta enviar a proceso 0
- ninguno esta recibiendo aun

Por eso el orden de las operaciones importa.

## 13. Contraste con memoria compartida

En memoria compartida:

- varios hilos acceden al mismo dato
- aparecen data races si no hay sincronizacion

En MPI:

- no hay variable compartida
- la comunicacion debe ser explicita

Esto simplifica ciertos razonamientos, aunque aumenta el costo de intercambio.

## 14. Problema aplicado: suma de los primeros `N` numeros

Una forma tipica de distribuir trabajo es:

1. el proceso 0 divide el rango
2. cada proceso suma su parte
3. cada proceso envia su suma parcial
4. el proceso 0 combina los resultados

### Esquema

```text
maestro:
  divide trabajo
  envia rangos
  recibe sumas parciales
  imprime total

trabajadores:
  reciben rango
  calculan suma parcial
  envian resultado al maestro
```

## 15. Ejemplo de pseudocodigo distribuido

```text
inicializar MPI
obtener rank y size

si rank == 0:
  dividir el rango 1..N entre los procesos
  para cada proceso trabajador:
    enviar inicio y fin del rango
  calcular su propia suma parcial
  recibir sumas parciales de todos
  sumar todo
  imprimir resultado
si no:
  recibir inicio y fin
  calcular suma parcial
  enviar suma parcial al proceso 0

finalizar MPI
```

## 16. Version en C con MPI

```c
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int N = 100;
    int start, end;
    int parcial = 0;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk = N / size;
    start = rank * chunk + 1;
    end = (rank == size - 1) ? N : (rank + 1) * chunk;

    for (int i = start; i <= end; i++) {
        parcial += i;
    }

    if (rank == 0) {
        int total = parcial;
        int recibido;

        for (int i = 1; i < size; i++) {
            MPI_Recv(&recibido, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            total += recibido;
        }

        printf("Suma total = %d\n", total);
    } else {
        MPI_Send(&parcial, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
```

## 17. Ideas clave para estudiar

- MPI usa procesos con memoria privada
- `MPI_Init` va al inicio y `MPI_Finalize` al final
- `rank` identifica al proceso actual
- `size` indica el total de procesos
- `MPI_Send` y `MPI_Recv` coordinan la comunicacion
- los tags deben coincidir
- `MPI_COMM_WORLD` incluye a todos los procesos
- el patron maestro-esclavo es muy comun
- el orden de envio y recepcion importa para evitar bloqueos
