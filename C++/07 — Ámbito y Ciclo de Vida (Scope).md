# 07 — Ámbito y Ciclo de Vida (Scope)

> **Resumen Ejecutivo:** El ámbito (scope) define la visibilidad y accesibilidad de un identificador en el código, mientras que el ciclo de vida (lifetime) dicta el periodo en el que una variable tiene memoria asignada en el Stack, Heap o segmento estático. Controlar ambos es vital para evitar memory leaks y bugs de acceso no autorizado.
>
> **Prerrequisitos:** Haber leído [06 — Wrappers y Gestión de Recursos (RAII)](06 — Wrappers y Gestión de Recursos (RAII).md).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Ámbitos y Duración de Almacenamiento](#teoría-ámbitos-y-duración-de-almacenamiento)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
- **Ámbito (Scope):** Región del código fuente donde un identificador (variable, función, clase) es válido y puede ser referenciado directamente.
- **Ciclo de vida (Lifetime):** Tiempo de ejecución durante el cual un objeto tiene garantizada una dirección física de memoria válida para almacenar su estado.

### ¿Por qué importa?
Vincular incorrectamente el ámbito y el ciclo de vida lleva a fallos de corrupción de memoria, como acceder a objetos cuya vida útil ha expirado (UAF - Use After Free) o perder variables activas sin haber liberado sus recursos (Memory Leak).

---

## Conceptos Previos
- Concepto de Stack (Pila de llamadas) y Heap (Montículo).
- Concepto de bloque delimitado por llaves `{}`.

---

## Hook Example

```cpp
#include <iostream>

void demostrar_scope() {
    int local_stack = 10; // Ámbito: Local de la función. Ciclo de vida: Hasta el fin de la función.
    
    {
        int anidado = 20; // Ámbito: Interno de las llaves. Ciclo de vida: Hasta la llave de cierre.
        std::cout << local_stack << " " << anidado << "\n";
    } // Aquí muere `anidado`
    
    // std::cout << anidado; // ERROR de compilación: no está en el ámbito actual.
}
```

---

## Descomposición Under the Hood

### Duración del almacenamiento en memoria (Storage Duration)
El enlazador y el compilador segmentan la memoria en tres áreas principales que controlan la vida útil física de los datos:
1. **Automático (Stack):** El puntero de pila (`RSP` en x86) disminuye para alojar las variables locales al entrar a la función y se incrementa al salir de ella. Su tiempo de vida es determinista e inmediato.
2. **Dinámico (Heap):** La memoria se reserva llamando a alocadores (`malloc` o `new`). Su tiempo de vida está controlado enteramente por el programador (o un smart pointer wrapper) y persiste hasta que se libera de forma manual.
3. **Estático / Global (Segmentos `.data` y `.bss`):** Las variables declaradas como `static` o globales se alocan una sola vez al cargar el ejecutable en memoria RAM. Su tiempo de vida se extiende desde el inicio hasta la terminación de la ejecución del programa.

---

## Teoría: Ámbitos y Duración de Almacenamiento

### Tipos de Ámbito (Scope)
- **Ámbito Local (Block Scope):** Cualquier variable declarada dentro de un bloque `{}`.
- **Ámbito de Clase:** Variables miembro y métodos definidos dentro de una declaración de clase.
- **Ámbito de Namespace:** Variables y clases agrupadas bajo un espacio de nombres (e.g. `namespace std`). Evita colisiones de nombres globales.
- **Ámbito Global:** Variables accesibles desde cualquier punto del programa (declaradas fuera de funciones, clases o namespaces).

### La palabra clave `static` y su impacto
1. **En variables locales:** Extiende el ciclo de vida de la variable para que persista entre llamadas a la función, pero manteniendo su ámbito estrictamente local a la función.
2. **En variables/funciones a nivel de archivo:** Restringe su vinculación para que solo sean visibles dentro de ese archivo de traducción específico (sustituido por namespaces anónimos en C++ moderno).

---

## Progresión de Complejidad

### Nivel Simple: Ocultación de nombres (Shadowing)
Declarar una variable local con el mismo nombre que una variable en un ámbito externo oculta temporalmente la variable del ámbito externo.
```cpp
int x = 5; // Global

int main() {
    int x = 10; // Local oculta a la global
    std::cout << "Local x: " << x << "\n"; // Imprime 10
    std::cout << "Global x: " << ::x << "\n"; // Acceso explícito a la global usando operador de resolución de ámbito (::)
    return 0;
}
```

### Nivel Aplicado: Variables locales estáticas (Static Local Variables)
Inicializadas exactamente una vez. Muy útiles para contadores o inicializaciones pesadas.
```cpp
int generar_id() {
    static int contador = 0; // Inicializado solo en la primera llamada
    return ++contador;
}
```

### Nivel Complejo: Lifetime Extender por Referencia Const
En C++, asignar un objeto temporal (Rvalue) a una referencia constante local (`const T&`) prolonga el ciclo de vida del objeto temporal hasta que la referencia misma salga de ámbito.
```cpp
#include <string>

std::string obtener_saludo() { return "Hola temporal"; }

int main() {
    {
        // El string temporal no se destruye al final de la expresión;
        // su tiempo de vida se prolonga para coincidir con el de `ref`.
        const std::string& ref = obtener_saludo();
        // usar ref de forma segura...
    } // Aquí es destruido el string temporal original
}
```

---

## Diseño de Sistemas
En el patrón de diseño Singleton, el ciclo de vida de una instancia única suele implementarse mediante una variable local estática dentro de un método estático de la clase, garantizando inicialización perezosa (lazy initialization) segura para hilos desde C++11 (Magic Statics).

---

## Ejercicios

### Ejercicio 1 — Analizar Variables Locales vs Estáticas
Completa la función `rastrear_visitas` para que mantenga una cuenta acumulada de cuántas veces ha sido llamada la función, y muestra su diferencia con una variable local no estática.

```cpp
#include <iostream>

void rastrear_visitas() {
    int local = 0;
    // TODO: Implementar una variable estática que acumule el conteo global de visitas
    static int estatica = 0;
    
    local++;
    estatica++;
    
    std::cout << "Local: " << local << ", Estatica: " << estatica << "\n";
}

int main() {
    rastrear_visitas(); // Debería imprimir: Local: 1, Estatica: 1
    rastrear_visitas(); // Debería imprimir: Local: 1, Estatica: 2
    rastrear_visitas(); // Debería imprimir: Local: 1, Estatica: 3
    return 0;
}
```

---

## Errores Comunes y Anti-Patrones
- **Retornar un puntero o referencia a una variable local Stack:**
  ```cpp
  int* obtener_direccion_invalida() {
      int temporal = 100;
      return &temporal; // ERROR: temporal se destruye al retornar. El puntero queda colgado.
  }
  ```
- **Confundir Scope y Lifetime:** Asumir que un objeto dinámico alocado con `new` se destruye porque el puntero que lo guardaba salió de ámbito. La dirección se pierde, pero el objeto dinámico sigue consumiendo memoria en el Heap (Memory Leak).

---

## Conclusión y Checklist Mental
- [ ] ¿Puedes explicar la diferencia entre Ámbito y Ciclo de vida?
- [ ] ¿Sabes qué efecto tiene la palabra clave `static` dentro de una función?
- [ ] ¿Entiendes qué es el operador de resolución de ámbito global (`::`)?

---

*Siguiente tema sugerido: [08 — Funciones y Operadores de Acceso](<08 — Funciones y Operadores de Acceso.md>)*


