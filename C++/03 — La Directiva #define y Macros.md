# 03 — La Directiva `#define` y Macros

> **Resumen Ejecutivo:** El preprocesador de C y C++ procesa directivas como `#define` antes de que el compilador real analice el código. Aunque fue la base de la metaprogramación en C, las macros introducen severos riesgos de seguridad y scope. C++ moderno provee alternativas superiores libres de efectos secundarios.
>
> **Prerrequisitos:** Haber leído [02 — Const en Variables y Funciones](<02 — Const en Variables y Funciones.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Macros vs Alternativas C++](#teoría-macros-vs-alternativas-c)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
La directiva `#define` es una instrucción de preprocesador que define un alias de reemplazo de texto en el código fuente. Puede actuar como constante de reemplazo o como macro con argumentos.

### ¿Por qué importa?
Las macros no respetan ámbitos (namespaces o clases), no tienen tipo asociado (carecen de type safety) y causan comportamientos inesperados cuando los argumentos tienen efectos secundarios (ej. operadores de incremento `++`).

---

## Conceptos Previos
- Qué es la etapa de preprocesamiento en la compilación.
- Concepto básico de reemplazo de texto.

---

## Hook Example

```cpp
// El peligro de las macros clásicas
#define CUADRADO(x) x * x

// Alternativa en C++ moderno
constexpr int cuadrado_seguro(int x) {
    return x * x;
}

int main() {
    int a = CUADRADO(2 + 2); // Esperado: 16. Realidad: 2 + 2 * 2 + 2 = 8 (prioridad de operadores)
    int b = cuadrado_seguro(2 + 2); // Garantizado: 16
    return 0;
}
```

---

## Descomposición Under the Hood

### ¿Qué hace el preprocesador?
- Antes de que el compilador genere la sintaxis (AST) o código de máquina:
  1. El preprocesador lee el archivo fuente línea por línea.
  2. Al encontrar `#define PI 3.14159`, reemplaza textualmente cada ocurrencia de la palabra `PI` por el texto literal `3.14159` antes del parseo de tipos.
  3. No se reservan direcciones en la tabla de símbolos del compilador para estas constantes macro. Esto hace imposible depurar el símbolo `PI` usando un debugger (gdb/lldb) porque el símbolo ya no existe en el binario compilado.

---

## Teoría: Macros vs Alternativas C++

C++ moderno reemplaza casi todo uso de `#define` mediante:
1. **Constantes:** `constexpr` y `const` en lugar de variables macro.
2. **Funciones en línea:** `inline` y `constexpr` funciones en vez de macros complejas.
3. **Genéricos:** Templates en lugar de macros de tipos genéricos.

### Comparación Directa:
| Aspecto | `#define` | `const` / `constexpr` |
|---|---|---|
| Chequeo de tipos | No (Reemplazo de texto) | Sí (Fuerte e inviolable) |
| Respeto de Scope | No (Global desde definición) | Sí (Bloque, Clase, Namespace) |
| Debuggeable | No (El compilador no ve la macro) | Sí (Aparece en la tabla de símbolos) |

---

## Progresión de Complejidad

### Nivel Simple: Reemplazo de constantes
```cpp
// Frágil y sin tipo
#define MAX_CONEXIONES 100

// Robusto y con tipo explícito
constexpr int max_conexiones = 100;
```

### Nivel Aplicado: Macros condicionales (Guarda de Cabeceras)
El único uso estándar remanente de las directivas de preprocesador es evitar inclusiones múltiples de archivos de cabecera.
```cpp
#ifndef MI_CLASE_H
#define MI_CLASE_H

class MiClase {};

#endif
```
*(Nota: En C++ moderno también se acepta `#pragma once` para este fin).*

### Nivel Complejo: Macros multilínea y efecto colateral
```cpp
#define INCREMENTAR_Y_ASIGNAR(destino, origen) \
    do { \
        destino = ++origen; \
    } while (0)
```
El uso de `do { ... } while(0)` es un truco de C clásico para permitir que la macro se comporte como una sentencia única que requiera un punto y coma final, evitando problemas en bloques `if/else`.

---

## Diseño de Sistemas
En el desarrollo de SDKs multiplataforma, las directivas de preprocesador (como `#ifdef _WIN32` o `#ifdef __linux__`) siguen siendo imprescindibles para compilar condicionalmente secciones de código específicas del sistema operativo.

---

## Ejercicios

### Ejercicio 1 — Reemplazar Macro por constexpr
Convierte las siguientes macros propensas a errores en código seguro de C++ usando `constexpr` o templates.

```cpp
#include <iostream>

// TODO: Reemplazar estas macros por funciones o constantes constexpr con tipos seguros
#define MULTIPLICAR(a, b) a * b
#define RUTA_DEFECTO "/usr/local/bin"

int main() {
    // Demuestra el fallo de la macro original llamándola con una suma:
    std::cout << MULTIPLICAR(5 + 3, 2) << "\n"; // Debería dar 16, pero da 11 con la macro.
    return 0;
}
```

---

## Errores Comunes y Anti-Patrones
- **No poner paréntesis en macros:** Definir `#define SUMA(a, b) a + b` causa UB en expresiones jerárquicas como `SUMA(1, 2) * 3`.
- **Efectos secundarios incrementales:** `CUADRADO(x++)` expande a `x++ * x++`, lo cual incrementa `x` dos veces y produce comportamiento indefinido en C++.

---

## Conclusión y Checklist Mental
- [ ] ¿Puedes explicar por qué las macros no respetan los `namespaces` de C++?
- [ ] ¿Entiendes por qué no puedes poner un breakpoint sobre una constante `#define` en un debugger?
- [ ] ¿Cuándo es estrictamente necesario usar preprocesador (ej. compilación condicional)?

---

*Siguiente tema sugerido: [04 — Punteros en C y C++](<04 — Punteros en C y C++.md>)*


