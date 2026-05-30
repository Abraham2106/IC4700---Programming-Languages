# Asignación y Semántica de Copia — Inicialización frente a asignación y la semántica de copia profunda

La asignación sobreescribe el estado de un objeto preexistente, a diferencia de la inicialización que lo construye desde cero. Distinguir la semántica de copia superficial de la profunda previene la compartición no intencionada de recursos en el heap.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
La asignación (`x = y`) consiste en sobreescribir el estado de un objeto previamente existente (`x`) con el estado de otro objeto (`y`). La semántica de copia dicta cómo se transfiere la propiedad o el valor de los recursos administrados.

### 1.2 ¿Por qué importa?
- **El Peligro de Compartir Punteros:** Por defecto, el compilador realiza una copia miembro a miembro (*member-wise copy*). Si la clase contiene punteros a recursos del sistema o memoria dinámica, ambos objetos compartirán el mismo recurso. Al destruirse el primer objeto, el recurso se liberará, dejando al segundo objeto con un puntero inválido (colgante) y provocando un fallo de doble liberación (*double free*) en su posterior destrucción.

---

## 2. Hook Example

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

## 3. Descomposición Under the Hood

### 3.1 Memoria Física en Copia Superficial (Shallow Copy)
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

### 3.2 Memoria Física en Copia Profunda (Deep Copy)
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

## 4. Teoría: Inicialización vs Asignación

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

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Comparadores Relacionales y Copia
Para que la semántica de copia sea consistente, el comportamiento de los comparadores (`operator==`, `operator!=`) debe validar la igualdad del valor profundo (el contenido del string), no la igualdad de las direcciones físicas de los punteros miembro.

### 5.2 Nivel Aplicado: Copias Híbridas (Copy-On-Write)
El libro *Secrets of C++ Masters* menciona estructuras donde se mezclan ambas copias. Varios objetos comparten inicialmente una copia superficial y un contador de referencias (optimización de recursos). Solo cuando uno de los objetos intenta escribir o modificar el contenido, se realiza la copia profunda (*Deep Copy*) del recurso físico.

### 5.3 Nivel Complejo: Depuración diagnóstica mediante Dump
En sistemas de bajo nivel, la única forma de rastrear fugas de memoria o punteros duplicados es a través de rutinas de volcado de bytes (`Dump`). Al emitir tanto las direcciones de los objetos del stack como las del heap, se puede auditar si se están cumpliendo los invariantes de unicidad de los punteros.

---

## 6. Diseño de Sistemas
En el diseño de lenguajes y sistemas, el paradigma de C++ ("Semántica de Valor por Defecto") contrasta con lenguajes como Java o C#, donde la asignación de objetos siempre copia la referencia de forma superficial. En C++, la semántica profunda es el núcleo de la seguridad de recursos y el determinismo de la destrucción.

---

## Exercises

### Exercise 1 — Simulación visual de punteros huérfanos
Dibuja el diagrama de memoria de stack y heap si ejecutamos `s2 = s1` sobre la clase del `Hook Example` eliminando el destructor. Señala cuántos bytes de memoria en el heap han quedado inaccesibles (fugados).

---

## 7. Errores Comunes y Anti-Patrones
- **Asumir que `T x = y;` invoca al operador de asignación:** Esto puede causar que código de limpieza crítico escrito en `operator=` no se ejecute.
- **Implementar una copia profunda pero dejar el destructor por defecto:** Causa fugas de los recursos internos creados por las copias profundas en el heap.

---

## 8. Conclusión

---

---

*Next: `25 — Asignacion con Clases y operator=.md` — Operador de asignación por copia y regla de tres/cinco.*
