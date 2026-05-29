# 34 — `extern` y Linkage en C++

> **Resumen Ejecutivo:** La palabra clave `extern` en C++ indica al compilador la declaración de una variable o función sin definirla, posponiendo la resolución de su dirección física a la fase de enlazado (*Linkage*). Asimismo, la directiva `extern "C"` desactiva el decorado de nombres (*name mangling*) de C++, permitiendo la interoperabilidad directa del enlazador con código compilado en lenguaje C o ensamblador. Esta nota analiza en profundidad las tablas de símbolos, los tipos de enlazado y el problema clásico del orden de inicialización estática (*Static Initialization Order Fiasco*).
>
> **Prerrequisitos:** Haber leído [07 — Ámbito y Ciclo de Vida (Scope)](<07 — Ámbito y Ciclo de Vida (Scope).md>) y [10 — Global Scope y Objetos Globales](<10 — Global Scope y Objetos Globales.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Enlazado (Linkage) y ODR](#teoría-enlazado-linkage-y-odr)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
- **Declaración vs Definición:** Una declaración introduce un símbolo y su tipo al compilador. Una definición asigna el espacio de almacenamiento real en memoria. `extern` permite declarar una variable global en un archivo de cabecera (`.h`) para que sea visible en múltiples archivos de origen, definiéndola físicamente en un único archivo fuente (`.cpp`).
- **Enlazado (Linkage):** El mecanismo por el cual el linker resuelve las referencias de símbolos compartidos a través de múltiples archivos binarios compilados individuales (archivos objeto `.obj` o `.o`).

### ¿Por qué importa?
- **Evitar la Regla de Definición Única (ODR):** Si defines una variable global (`int g_valor = 0;`) en un archivo de cabecera que se incluye en tres archivos `.cpp`, el linker abortará el proceso con un error de "símbolo duplicado". Declararlo como `extern` previene este fallo.
- **Interoperabilidad de APIs:** Las bibliotecas de C++ decoran los nombres de las funciones para soportar sobrecarga. Para llamar a una función compilada en C (que usa nombres planos), se requiere `extern "C"` para desactivar esta decoración.

---

## Conceptos Previos
- Comprensión del enlazador (linker) frente al compilador.
- La diferencia entre variables globales de ámbito de archivo y de ámbito de bloque.

---

## Hook Example

```cpp
#include <iostream>

// --- En ArchivoGlobal.h (simulado) ---
// Declaramos la existencia de una variable global sin reservar memoria
extern int g_contadorLlamadas; 

// Declaramos una funcion con enlazado C para evitar name mangling
extern "C" void funcionEnC(int x);

// --- En ArchivoGlobal.cpp (simulado) ---
// Definimos físicamente la variable y reservamos espacio
int g_contadorLlamadas = 0; 

extern "C" void funcionEnC(int x) {
    std::cout << "Llamada desde C con valor: " << x << "\n";
    g_contadorLlamadas++;
}

// --- En Main.cpp ---
int main() {
    funcionEnC(42);
    funcionEnC(100);

    // Accedemos a la variable externa declarada
    std::cout << "Total de llamadas registradas: " << g_contadorLlamadas << "\n";

    return 0;
}
```

---

## Descomposición Under the Hood

### Decorado de Nombres (Name Mangling) y la interoperabilidad binaria
C++ permite la sobrecarga de funciones (mismo nombre, diferentes argumentos).
- Para que el linker pueda diferenciar qué función exacta invocar, el compilador genera nombres únicos decorados codificando la firma de la función en la tabla de símbolos del archivo objeto `.obj`.
- Por ejemplo, la función `void procesar(int, double)` puede ser decorada por el compilador como `?procesar@@YAXHN@Z`.
- C no soporta sobrecarga de funciones y guarda los nombres exactamente como fueron declarados: `_procesar`.
- **El rol de `extern "C"`:** Le indica al compilador de C++ que no decore los nombres de las funciones contenidas en ese bloque, forzando la compatibilidad de nombres planos de C, permitiendo enlazar directamente módulos binarios escritos en C o ensamblador.

---

## Teoría: Enlazado (Linkage) y ODR

C++ clasifica el enlazado en tres tipos principales:

### 1. External Linkage (Enlazado Externo)
El nombre puede ser accedido y resuelto desde otros archivos de código fuente. Es el comportamiento por defecto para variables globales no constantes y funciones normales.

### 2. Internal Linkage (Enlazado Interno)
El símbolo es estrictamente local al archivo `.cpp` actual en el que se define. Otros archivos compilados pueden declarar nombres idénticos sin entrar en conflicto.
- Se logra declarando el símbolo como `static` a nivel global o colocándolo dentro de un **espacio de nombres anónimo** (`namespace { ... }`).
- Las variables globales marcadas como `const` o `constexpr` tienen, por defecto, enlazado interno en C++.

### 3. No Linkage
El nombre solo existe y se puede referenciar dentro de su bloque de código local (ej. variables locales dentro de una función).

---

## Progresión de Complejidad

### Nivel Simple: variables `const` vs globales estándar
Declarar `int x;` en dos archivos independientes causa colisión de enlazado externo. Para aislar variables sin crear tipos de miembros estáticos, se declaran globales estáticas (`static int local;`), limitando su enlazabilidad al archivo fuente local.

### Nivel Aplicado: Static Initialization Order Fiasco
El libro *Secrets of C++ Masters* advierte sobre un bug catastrófico en sistemas con variables globales distribuidas en múltiples archivos `.cpp` (unidades de traducción).
- El estándar de C++ garantiza el orden de construcción de variables globales dentro del mismo archivo fuente, pero **no define ningún orden de inicialización entre archivos fuente diferentes**.
- Si una variable global externa `extern ObjetoA g_a;` en `A.cpp` es desreferenciada por el constructor de otra variable global `ObjetoB g_b;` en `B.cpp` durante la carga del ejecutable, y el enlazador decide inicializar primero el archivo `B.cpp` antes que `A.cpp`:
- El constructor de `g_b` intentará leer la memoria de un objeto `g_a` que aún contiene puros ceros físicos en su estructura, provocando fallas imprevistas.
- **Solución clásica:** Envolver el objeto en una función estática local (patrón Singleton con inicialización perezosa):
```cpp
ObjetoA& getGlobalA() {
    static ObjetoA instancia; // Inicializado en el primer uso de forma garantizada
    return instancia;
}
```

### Nivel Complejo: Cabeceras híbridas C y C++
Al diseñar bibliotecas para ser consumidas por ambos lenguajes, se protegen las declaraciones con macros de preprocesador específicas de C++ para aplicar `extern "C"` únicamente cuando el compilador sea de C++:
```cpp
#ifdef __cplusplus
extern "C" {
#endif

void miFuncionDeBiblioteca(int);

#ifdef __cplusplus
}
#endif
```

---

## Diseño de Sistemas
En arquitectura de sistemas multiplataforma o modulares (como el desarrollo de plugins dinámicos `.dll` o `.so`), `extern "C"` es el único mecanismo confiable para exponer funciones de punto de entrada estables de la API, ya que los compiladores de C++ de diferentes fabricantes decoran los nombres de funciones con esquemas binarios incompatibles entre sí (incompatibilidad ABI).

---

## Ejercicios

### Ejercicio 1 — Simulación del fallo de orden de inicialización
Crea dos archivos conceptuales `.cpp`. En el primero define un puntero global inicializado a un objeto válido. En el segundo define un objeto global cuyo constructor use `extern` para desreferenciar el puntero del primer archivo. Explica en qué condiciones de compilación este código provocará un crash en ejecución.

---

## Errores Comunes y Anti-Patrones
- **Definir la variable en la cabecera:** Escribir `extern int x = 10;` en un archivo `.h`. Esto se interpreta como una definición (debido a la asignación de valor), anulando el propósito de `extern` y produciendo errores de duplicación al ser incluido en múltiples sitios.
- **Olvidar `extern "C"` en callbacks de sistemas operativos de bajo nivel:** Provoca caídas o fallos de enlazado ya que la API externa (usualmente escrita en C) no podrá resolver el nombre decorado generado por el compilador de C++.

---

## Conclusión y Checklist Mental
- [ ] ¿Cuál es la diferencia exacta entre declarar y definir una variable global usando `extern`?
- [ ] ¿Cómo ayuda la directiva `extern "C"` a integrar código fuente de C++ con librerías legadas de C?
- [ ] ¿Qué es el "Static Initialization Order Fiasco" y cómo se soluciona de forma definitiva?
- [ ] ¿Por qué el compilador de C++ realiza el decorado de nombres (*name mangling*)?

---

*Siguiente tema sugerido: [01 — Diferencias entre C, C++ y Rust](<01 — Diferencias entre C, C++ y Rust.md>)*
