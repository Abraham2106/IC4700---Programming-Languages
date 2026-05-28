# 18 — Construcción por Copia y Movimiento

> **Resumen Ejecutivo:** Controlar cómo se copian y mueven los objetos en C++ es crucial para evitar fugas de memoria y optimizar el rendimiento. La "Regla de los Tres/Cinco/Cero" define cuándo debemos programar manualmente los constructores de copia, movimiento y sus respectivos operadores de asignación.
>
> **Prerrequisitos:** Haber leído [05 — Referencias en C++](<05 — Referencias en C++.md>) y [17 — Constructores en C++](<17 — Construcción por Copia y Movimiento.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Ciclo de Vida de Copia y Movimiento](#teoría-ciclo-de-vida-de-copia-y-movimiento)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
Define el comportamiento del objeto ante dos tipos de asignación:
1. **Copia:** Duplicar el contenido de un objeto en otra variable independiente.
2. **Movimiento (C++11):** Transferir la propiedad de los recursos de un objeto temporal a otro, dejando el original vacío pero en estado válido.

### ¿Por qué importa?
Por defecto, el compilador realiza una copia bit a bit (shallow copy) de los miembros del objeto. Si tu objeto contiene un puntero a memoria Heap, ambos objetos terminarán apuntando a la misma dirección física. Al destruirse, ambos llamarán a `delete` sobre el mismo puntero, provocando un error de doble liberación (Double Free) y corrupción de memoria RAM.

---

## Conceptos Previos
- Diferencia general entre Lvalues y Rvalues.
- Qué es memoria dinámica (Heap).

---

## Hook Example

```cpp
#include <iostream>
#include <utility>

class Buffer {
    int* datos;
    int tamano;
public:
    Buffer(int sz) : tamano(sz), datos(new int[sz]{0}) {}
    ~Buffer() { delete[] datos; }

    // 1. Constructor de Copia (Copia Profunda / Deep Copy)
    Buffer(const Buffer& otro) : tamano(otro.tamano), datos(new int[otro.tamano]) {
        std::copy(otro.datos, otro.datos + tamano, datos);
        std::cout << "Copia profunda realizada.\n";
    }

    // 2. Constructor de Movimiento (Roba recursos)
    Buffer(Buffer&& otro) noexcept : datos(otro.datos), tamano(otro.tamano) {
        otro.datos = nullptr; // Deja al temporal en estado seguro
        otro.tamano = 0;
        std::cout << "Recurso movido sin copias.\n";
    }
};

int main() {
    Buffer b1(100);
    Buffer b2 = b1; // Invoca Constructor de Copia
    Buffer b3 = std::move(b1); // Invoca Constructor de Movimiento
    return 0;
}
```

---

## Descomposición Under the Hood

### ¿Qué ocurre a nivel de CPU y Memoria?
- **Copia Profunda (Deep Copy):** La CPU debe alocar un nuevo bloque físico de memoria en el Heap (llamada a `malloc` / `new`) y luego iterar a través de las celdas copiando físicamente los bits de datos uno por uno. Esto consume ciclos de reloj y causa latencia si el buffer es grande.
- **Movimiento (Move):** Es una operación instantánea ($O(1)$). La CPU simplemente copia el valor numérico de la dirección de memoria guardada en el puntero del objeto origen al puntero del objeto destino, y luego sobrescribe el puntero origen con `nullptr`. No se aloca memoria nueva en el Heap ni se copian datos físicos de búferes.

---

## Teoría: Ciclo de Vida de Copia y Movimiento

### 1. Las Cinco Funciones Especiales (The Rule of 5)
Si manejas manualmente recursos (como punteros crudos), probablemente debas implementar o declarar el comportamiento de estas 5 funciones:
1. **Destructor:** Libera recursos.
2. **Constructor de Copia (`T(const T&)`):** Duplica recursos.
3. **Operador de Asignación por Copia (`T& operator=(const T&)`):** Libera recursos actuales y duplica los del otro.
4. **Constructor de Movimiento (`T(T&&)`):** Adquiere recursos del temporal.
5. **Operador de Asignación por Movimiento (`T& operator=(T&&)`):** Libera recursos actuales y adquiere los del temporal.

### 2. La Regla del Cero (Rule of 0)
Intenta evitar diseñar clases que controlen recursos manualmente. Si utilizas tipos RAII estándar (como `std::vector`, `std::unique_ptr`, `std::string`) dentro de tus atributos, el compilador autogenerará copias y movimientos correctos automáticamente, reduciendo tu trabajo a cero.

---

## Progresión de Complejidad

### Nivel Simple: Copia vs Asignación
```cpp
Buffer b1(10);
Buffer b2 = b1; // Construcción por copia (el objeto b2 está naciendo)

Buffer b3(5);
b3 = b2; // Operador de asignación por copia (b3 ya existía, debe liberar su memoria previa antes)
```

### Nivel Aplicado: Garantía de no lanzamiento (`noexcept`)
El constructor y la asignación por movimiento siempre deben marcarse como `noexcept` (sin excepciones). Si no lo haces, los contenedores como `std::vector` preferirán copiar tus objetos en lugar de moverlos durante las operaciones de redimensionamiento interno por motivos de seguridad.
```cpp
Buffer(Buffer&& otro) noexcept;
```

---

## Diseño de Sistemas
La semántica de movimiento revolucionó C++ (a partir de C++11) al permitir que funciones retornen grandes colecciones por valor (ej. `std::vector<std::string> obtener_datos()`) sin penalizaciones de rendimiento, ya que el compilador simplemente mueve la cabecera de punteros en lugar de copiar la memoria subyacente.

---

## Ejercicios

### Ejercicio 1 — Implementar la Regla de los Tres (Copying Rule)
Completa los métodos de copia e intercambio de la clase `Cadena` para evitar fallos de Double Free en runtime.

```cpp
#include <iostream>
#include <cstring>

class Cadena {
    char* buffer;
public:
    explicit Cadena(const char* txt) {
        buffer = new char[std::strlen(txt) + 1];
        std::strcpy(buffer, txt);
    }
    ~Cadena() { delete[] buffer; }

    // TODO: Implementar Constructor de Copia
    Cadena(const Cadena& otra) {
        // Implementar copia profunda
    }

    // TODO: Implementar Operador de Asignación por Copia
    Cadena& operator=(const Cadena& otra) {
        if (this != &otra) {
            // Liberar memoria actual, alocar nueva y copiar
        }
        return *this;
    }
};

int main() {
    Cadena c1("Prueba");
    Cadena c2 = c1; // Debe llamar al constructor de copia
    c2 = c1; // Debe llamar al operador de asignación
    return 0;
}
```

---

## Errores Comunes y Anti-Patrones
- **Autoasignación desatendida:** En el operador de asignación (`operator=`), olvidar comprobar si te estás asignando a ti mismo (`this == &otra`). Esto causa que liberes tu propia memoria antes de intentar copiarla, resultando en corrupción inmediata.
- **Olvidar resetear el puntero origen en movimiento:** No poner a `nullptr` el puntero del objeto temporal origen tras mover sus recursos causará que su destructor posterior libere la memoria que el objeto destino acaba de heredar.

---

## Conclusión y Checklist Mental
- [ ] ¿Entiendes por qué una copia bit a bit (shallow copy) causa Double Free si la clase maneja memoria dinámica?
- [ ] ¿Qué beneficio aporta marcar las funciones de movimiento como `noexcept`?
- [ ] ¿Cuál es la diferencia entre el constructor de copia y el operador de asignación?

---

*Siguiente tema sugerido: [19 — Gestión de Memoria Stack vs Heap](<19 — Operadores new y delete.md>)*





