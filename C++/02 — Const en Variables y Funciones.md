# 02 — Const en Variables y Funciones

> **Resumen Ejecutivo:** El uso de `const` en C++ es un contrato de inmutabilidad en tiempo de compilación. Entender sus matices sobre variables, punteros, referencias y métodos es fundamental para escribir código seguro, optimizable y con correctitud de const (const correctness).
>
> **Prerrequisitos:** Haber leído [01 — Diferencias entre C, C++ y Rust](<C vs C++ vs Rust.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Const y Constexpr](#teoría-const-y-constexpr)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
`const` es una palabra clave en C++ que designa a un objeto o variable como inmutable. Una vez inicializado, su valor no puede ser modificado. No obstante, en C++, `const` se puede aplicar en múltiples niveles: variables locales, parámetros de función, tipos de retorno, punteros y métodos de clase.

### ¿Por qué importa?
1. **Seguridad:** Evita que modifiques accidentalmente datos que deberían ser de solo lectura.
2. **Optimizaciones del compilador:** El compilador puede realizar optimizaciones agresivas si sabe que un valor no cambiará.
3. **Legibilidad:** Actúa como documentación activa en el código.

---

## Conceptos Previos
- Qué es una dirección de memoria.
- Qué es el tiempo de compilación vs el tiempo de ejecución.

---

## Hook Example

```cpp
// C++: El poder de const correctness
class Persona {
    std::string nombre;
public:
    Persona(std::string n) : nombre(n) {}
    
    // Método const: garantiza que no modifica el estado del objeto
    std::string get_nombre() const { 
        // nombre = "Otro"; // ERROR: El compilador lo prohíbe.
        return nombre; 
    }
};

void imprimir(const Persona& p) {
    // Al recibir por const reference, solo podemos llamar métodos const
    std::cout << p.get_nombre() << "\n"; 
}
```

---

## Descomposición Under the Hood

### ¿Qué hace el compilador con `const`?
- **Type Safety en Compile-time:** `const` es principalmente una instrucción para el compilador. El compilador mantendrá un registro del tipo y marcará cualquier intento de modificación directa como un error de compilación.
- **Optimización:** En variables simples, el compilador puede reemplazar el uso de la variable directamente por su valor literal en el código ensamblador (inline expansion), eliminando lecturas a memoria.
- **Secciones de Solo Lectura:** Para variables globales `const`, el compilador y el enlazador pueden colocarlas en la sección `.rodata` del ejecutable. A nivel de hardware, intentar escribir en esta sección provoca una violación de acceso o fallo de página (Segmentation Fault).

---

## Teoría: Const y Constexpr

### 1. Variables const
- `const int x = 10;` define un entero inmutable. Debe ser inicializado al declararse.

### 2. Punteros y const (La regla de lectura de derecha a izquierda)
- `const int* p;` o `int const* p;`: Puntero a un entero constante. El entero no se puede modificar, pero el puntero sí puede cambiar de dirección.
- `int* const p = &x;`: Puntero constante a un entero. El entero se puede modificar, pero el puntero no puede apuntar a otra dirección.
- `const int* const p = &x;`: Puntero constante a un entero constante. Nada puede cambiar.

### 3. Métodos const
- Declarar un método como `const` dentro de una clase (e.g., `void mostrar() const;`) cambia el tipo implícito del puntero `this` de `T* const` a `const T* const`. Esto impide modificar cualquier variable miembro no estática del objeto desde el método.

### 4. `constexpr` vs `const`
- `const` significa "de solo lectura". Su valor puede decidirse en tiempo de ejecución (ej. la entrada del usuario).
- `constexpr` (C++11) significa "constante en tiempo de compilación". Obliga a que el valor sea computable al compilar.

---

## Progresión de Complejidad

### Nivel Simple: Parámetro const por valor vs referencia
```cpp
// Copia inútil si el objeto es grande, pero segura.
void func1(const std::string s); 

// Óptimo: evita copias y garantiza inmutabilidad.
void func2(const std::string& s); 
```

### Nivel Aplicado: mutable keyword
A veces necesitamos cambiar un miembro interno dentro de un método `const` (ej. mutexes o contadores de caché). C++ provee la palabra clave `mutable`.
```cpp
class Canal {
    mutable std::mutex mtx;
    std::string datos;
public:
    std::string leer() const {
        std::lock_guard<std::mutex> lock(mtx); // Legal porque mtx es mutable
        return datos;
    }
};
```

### Nivel Complejo: `const_cast`
C++ permite remover la propiedad `const` temporalmente usando un cast explícito, aunque modificar un objeto originalmente creado como `const` a través de esto produce comportamiento indefinido (UB).
```cpp
void modificar_sucio(const int* p) {
    int* no_const = const_cast<int*>(p);
    *no_const = 100; // Cuidado: Solo legal si el objeto original no era const
}
```

---

## Diseño de Sistemas
En el diseño de bibliotecas de alto rendimiento, pasar objetos grandes por `const&` (referencia constante) es la convención estándar para evitar sobrecarga por copias de memoria en el Stack.

---

## Ejercicios

### Ejercicio 1 — Implementar Const Correctness
Corrige los errores de compilación del siguiente código aplicando `const` adecuadamente en los métodos del modelo.

```cpp
#include <iostream>
#include <vector>

class Inventario {
    std::vector<std::string> items;
public:
    void agregar(const std::string& item) { items.push_back(item); }
    // TODO: Hacer este método const para poder llamarlo en imprimir_inventario
    size_t obtener_tamano() { return items.size(); }
};

// TODO: Cambiar el parámetro para que sea const Inventario& y corregir lo necesario
void imprimir_inventario(Inventario& inv) {
    std::cout << "Items: " << inv.obtener_tamano() << "\n";
}
```

---

## Errores Comunes y Anti-Patrones
- **Retornar `const` por valor en tipos primitivos:** `const int get_valor();` no tiene utilidad real y puede deshabilitar optimizaciones de movimiento en clases complejas.
- **Modificar objetos `const` con `const_cast`:** Si el objeto fue declarado originalmente como `const`, forzar su escritura causará un crash en runtime si reside en memoria `.rodata`.

---

## Conclusión y Checklist Mental
- [ ] ¿Entiendes la diferencia entre `const int*` e `int* const`?
- [ ] ¿Sabes por qué los métodos que no modifican el estado de la clase siempre deben marcarse como `const`?
- [ ] ¿Diferencias con claridad `const` de `constexpr`?

---

*Siguiente tema sugerido: [03 — La Directiva #define y Macros](<03 — La Directiva #define y Macros.md>)*


