# 24 — Asignación y Semántica de Copia

> **Resumen Ejecutivo:** La asignación en C++ es una operación fundamental pero propensa a errores cuando se trabaja con tipos no triviales. Esta nota analiza las diferencias entre la inicialización y la asignación pura, detalla el comportamiento de copias superficiales (*Shallow Copy*), copias profundas (*Deep Copy*) y copias híbridas, y provee herramientas de diagnóstico basadas en volcados (*Dumping*) de bytes y punteros.
>
> **Prerrequisitos:** Haber leído [04 — Punteros en C y C++](<04 — Punteros en C y C++.md>), [05 — Referencias en C++](<05 — Referencias en C++.md>), y [18 — Construcción por Copia y Movimiento](<18 — Construcción por Copia y Movimiento.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Inicialización vs Asignación](#teoría-inicialización-vs-asignación)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
La asignación (`x = y`) consiste en sobreescribir el estado de un objeto previamente existente (`x`) con el estado de otro objeto (`y`). La semántica de copia dicta cómo se transfiere la propiedad o el valor de los recursos administrados.

### ¿Por qué importa?
- **El Peligro de Compartir Punteros:** Por defecto, el compilador realiza una copia miembro a miembro (*member-wise copy*). Si la clase contiene punteros a recursos del sistema o memoria dinámica, ambos objetos compartirán el mismo recurso. Al destruirse el primer objeto, el recurso se liberará, dejando al segundo objeto con un puntero inválido (colgante) y provocando un fallo de doble liberación (*double free*) en su posterior destrucción.

---

## Conceptos Previos
- Diferencia entre la pila (stack) y el montículo (heap).
- Modificador `const` en referencias y punteros.
- Qué es la desreferenciación e indirección de direcciones de memoria.

---

## Hook Example

```cpp
#include <iostream>
#include <cstring>
#include <iomanip>

class StringSeguro {
    char* datos;
public:
    explicit StringSeguro(const char* txt) {
        datos = new char[std::strlen(txt) + 1];
        std::strcpy(datos, txt);
    }

    ~StringSeguro() {
        delete[] datos;
    }

    // Constructor de copia para Deep Copy
    StringSeguro(const StringSeguro& otro) {
        datos = new char[std::strlen(otro.datos) + 1];
        std::strcpy(datos, otro.datos);
    }

    // Metodo Dump para diagnóstico profundo de memoria
    void Dump(std::ostream& os) const {
        os << "[OBJETO] Direccion del wrapper en Stack: " << this << "\n"
           << "         Direccion de 'datos' en Heap: " << (void*)datos << "\n"
           << "         Contenido: \"" << datos << "\"\n"
           << "         Bytes en hexadecimal: ";
        
        // Volcado de bytes hexadecimales del recurso
        size_t len = std::strlen(datos) + 1;
        for (size_t i = 0; i < len; ++i) {
            os << std::hex << std::setw(2) << std::setfill('0') 
               << (int)(unsigned char)datos[i] << " ";
        }
        os << std::dec << "\n";
    }
};

int main() {
    StringSeguro s1("Hello");
    StringSeguro s2("Goodbye");

    std::cout << "--- Estado Inicial ---\n";
    s1.Dump(std::cout);
    s2.Dump(std::cout);

    // Si hicieramos 's2 = s1' sin sobrecargar operator=, ocurriria copia superficial:
    // s2.datos apuntaría a s1.datos, y la memoria original de "Goodbye" se perdería.
    
    std::cout << "--- Creando copia (Constructor de copia) ---\n";
    StringSeguro s3 = s1; // Construccion por copia (Deep Copy)
    s3.Dump(std::cout);

    return 0;
}
```

---

## Descomposición Under the Hood

### Memoria Física en Copia Superficial (Shallow Copy)
Si no se redefine el comportamiento de copia, el compilador realiza una copia cruda de bits:

```
STACK                               HEAP
┌──────────┐                        ┌──────────────┐
│ s1       │                        │              │
│  datos ──┼───────────────────────>│ "Hello" \0   │
└──────────┘                        │              │
                                    └──────────────┘
┌──────────┐                               ▲
│ s2       │                               │
│  datos ──┼───────────────────────────────┘
└──────────┘
(Ambos punteros apuntan al mismo bloque. Se pierde el bloque original de s2. Leaks y Double Free asegurados)
```

### Memoria Física en Copia Profunda (Deep Copy)
Al implementar semántica de valor profundo, se independizan los recursos:

```
STACK                               HEAP
┌──────────┐                        ┌──────────────┐
│ s1       │                        │              │
│  datos ──┼───────────────────────>│ "Hello" \0   │
└──────────┘                        │              │
                                    └──────────────┘
┌──────────┐                        ┌──────────────┐
│ s2       │                        │              │
│  datos ──┼───────────────────────>│ "Hello" \0   │ (Bloque duplicado físico)
└──────────┘                        │              │
                                    └──────────────┘
```

---

## Teoría: Inicialización vs Asignación

Es común confundir la inicialización con la asignación debido al uso del signo `=`. El compilador de C++ las diferencia de forma estricta:

1. **Inicialización (Construcción):** Ocurre cuando el objeto está naciendo en memoria por primera vez.
   ```cpp
   StringSeguro s1("Hello");
   StringSeguro s2 = s1;       // Llama al CONSTRUCTOR DE COPIA. s2 no existía en memoria.
   StringSeguro s3(s1);        // Llama al CONSTRUCTOR DE COPIA. Sintaxis alternativa equivalente.
   ```
2. **Asignación:** Ocurre sobre un objeto completamente construido que ya posee recursos en ejecución.
   ```cpp
   StringSeguro s1("Hello");
   StringSeguro s2("Goodbye"); // Ya construido
   s2 = s1;                    // Llama al OPERADOR DE ASIGNACIÓN (operator=).
   ```

---

## Progresión de Complejidad

### Nivel Simple: Comparadores Relacionales y Copia
Para que la semántica de copia sea consistente, el comportamiento de los comparadores (`operator==`, `operator!=`) debe validar la igualdad del valor profundo (el contenido del string), no la igualdad de las direcciones físicas de los punteros miembro.

### Nivel Aplicado: Copias Híbridas (Copy-On-Write)
El libro *Secrets of C++ Masters* menciona estructuras donde se mezclan ambas copias. Varios objetos comparten inicialmente una copia superficial y un contador de referencias (optimización de recursos). Solo cuando uno de los objetos intenta escribir o modificar el contenido, se realiza la copia profunda (*Deep Copy*) del recurso físico.

### Nivel Complejo: Depuración diagnóstica mediante Dump
En sistemas de bajo nivel, la única forma de rastrear fugas de memoria o punteros duplicados es a través de rutinas de volcado de bytes (`Dump`). Al emitir tanto las direcciones de los objetos del stack como las del heap, se puede auditar si se están cumpliendo los invariantes de unicidad de los punteros.

---

## Diseño de Sistemas
En el diseño de lenguajes y sistemas, el paradigma de C++ ("Semántica de Valor por Defecto") contrasta con lenguajes como Java o C#, donde la asignación de objetos siempre copia la referencia de forma superficial. En C++, la semántica profunda es el núcleo de la seguridad de recursos y el determinismo de la destrucción.

---

## Ejercicios

### Ejercicio 1 — Simulación visual de punteros huérfanos
Dibuja el diagrama de memoria de stack y heap si ejecutamos `s2 = s1` sobre la clase del `Hook Example` eliminando el destructor. Señala cuántos bytes de memoria en el heap han quedado inaccesibles (fugados).

---

## Errores Comunes y Anti-Patrones
- **Asumir que `T x = y;` invoca al operador de asignación:** Esto puede causar que código de limpieza crítico escrito en `operator=` no se ejecute.
- **Implementar una copia profunda pero dejar el destructor por defecto:** Causa fugas de los recursos internos creados por las copias profundas en el heap.

---

## Conclusión y Checklist Mental
- [ ] ¿Por qué `x = y` no es una inicialización si `x` ya existía?
- [ ] ¿Cuál es la diferencia de comportamiento entre copiar un puntero (dirección) y copiar el recurso al que apunta?
- [ ] ¿Para qué sirve implementar un método `Dump` en fases de depuración de punteros?
- [ ] ¿Qué ocurre con la memoria del objeto izquierdo en una asignación sin limpieza?

---

*Siguiente tema sugerido: [25 — Asignación con Clases y operator=](<25 — Asignacion con Clases y operator=.md>)*
