# 14 — Herencia Privada y Composición

> **Resumen Ejecutivo:** La reutilización de código en C++ se logra principalmente a través de la Composición ("tiene-un") y la Herencia Privada ("se implementa en términos de"). Aunque sintácticamente diferentes, ambas técnicas sirven para estructurar software sin exponer detalles internos al usuario.
>
> **Prerrequisitos:** Haber leído [12 — Herencia y Visibilidad](<12 — Herencia y Visibilidad.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Composición vs Herencia Privada](#teoría-composición-vs-herencia-privada)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
- **Composición:** Crear una clase que contiene variables miembro de otros tipos (e.g., la clase `Auto` contiene una instancia de `Motor`). Relación *"tiene-un"* (has-a).
- **Herencia Privada:** Declarar herencia usando la palabra clave `private` (e.g. `class A : private B`). Relación *"se implementa en términos de"* (implemented-in-terms-of). Evita que el usuario final convierta punteros de la clase derivada a la clase base en código externo.

### ¿Por qué importa?
Una de las reglas de oro de la orientación a objetos es **"Prefiere composición sobre herencia"** para mantener acoplamientos débiles. No obstante, C++ provee herencia privada para escenarios de optimización específicos, como acceder a miembros protegidos de una base o sobrescribir funciones virtuales de utilidades sin exponer la jerarquía.

---

## Conceptos Previos
- Diferencia general entre herencia pública y composición.

---

## Hook Example

```cpp
#include <iostream>

class Motor {
public:
    void arrancar() { std::cout << "Motor en marcha.\n"; }
};

// Composición (Recomendado por defecto)
class AutoComposicion {
    Motor motor; // tiene-un
public:
    void encender() { motor.arrancar(); }
};

// Herencia Privada (Oculta la interfaz de Motor)
class AutoHerenciaPrivada : private Motor { // implementado en términos de
public:
    void encender() { arrancar(); } // Acceso legal a funciones de Motor
};

int main() {
    AutoComposicion a1;
    a1.encender();

    AutoHerenciaPrivada a2;
    a2.encender();
    // Motor* m = &a2; // ERROR: La conversión implícita a la clase base está prohibida con herencia privada.
    return 0;
}
```

---

## Descomposición Under the Hood

### Optimización del Miembro Vacío (Empty Base Optimization - EBO)
- Si usas composición y tienes una clase vacía `class Util {};` (de 0 bytes lógicos) y la agregas a otra clase:
  ```cpp
  class Contenedor {
      Util u; // Ocupará al menos 1 byte en el struct para garantizar dirección de memoria única.
      int datos;
  }; // sizeof(Contenedor) dará 8 bytes debido a alineación (padding).
  ```
- Si usas herencia privada en su lugar:
  ```cpp
  class Contenedor : private Util {
      int datos;
  }; // sizeof(Contenedor) dará 4 bytes exactos.
  ```
- C++ aplica **EBO** sobre clases bases vacías heredadas, eliminando por completo el byte de relleno en la estructura del objeto derivado.

---

## Teoría: Composición vs Herencia Privada

### 1. Composición ("Tiene-un"):
- Los objetos contenidos son cajas negras independientes.
- Menor acoplamiento.
- Permite cambiar la instancia contenida dinámicamente en tiempo de ejecución.
- Recomendada para el 95% de los casos de reutilización.

### 2. Herencia Privada ("Se implementa en términos de"):
- Oculta por completo la interfaz pública de la clase base.
- Permite sobrescribir métodos virtuales (`virtual`) de la clase base.
- Permite acceder a miembros protegidos (`protected`) de la clase base.
- Habilita la optimización EBO.

---

## Progresión de Complejidad

### Nivel Simple: Composición clásica
```cpp
class Pantalla {};
class Teclado {};

class Computadora {
    Pantalla p;
    Teclado t;
};
```

### Nivel Aplicado: Redefinición selectiva de interfaz (Using declaration)
Podemos re-exponer selectivamente funciones de la base privada usando la palabra clave `using`.
```cpp
class ListaBase {
public:
    void insertar(int x) {}
    void borrar_todo() {}
};

class ColaEspecial : private ListaBase {
public:
    // Exponer públicamente solo la inserción
    using ListaBase::insertar; 
};
```

### Nivel Complejo: Sobrescribir comportamiento privado (Polimorfismo Oculto)
Permite usar una biblioteca que nos pide heredar para implementar un callback, sin exponer esa relación de herencia a nuestros usuarios finales.
```cpp
#include <iostream>

class TemporizadorCallback {
public:
    virtual void al_terminar() = 0;
};

// Heredamos privadamente para redefinir al_terminar, pero nadie fuera de MiServicio sabrá que somos un Callback
class MiServicio : private TemporizadorCallback {
private:
    void al_terminar() override {
        std::cout << "Servicio reaccionando al temporizador.\n";
    }
};
```

---

## Diseño de Sistemas
En el diseño de contenedores de alto rendimiento (como `std::vector`), se suele heredar privadamente de un alocador de memoria vacío para aprovechar la optimización EBO, evitando gastar bytes innecesarios en la estructura del contenedor.

---

## Ejercicios

### Ejercicio 1 — Implementar una Pila vía Herencia Privada de std::vector
Crea una clase `Pila` que herede privadamente de `std::vector<int>` y exponga únicamente los métodos `push(int)`, `pop()`, y `size()` usando declaraciones `using` o envoltorios simples.

```cpp
#include <iostream>
#include <vector>

// TODO: Heredar privadamente de std::vector<int>
class Pila {
public:
    // TODO: Exponer selectivamente o envolver:
    // void push(int x);
    // void pop();
    // size_t size() const;
};

int main() {
    Pila p;
    // p.push(10);
    // p.push(20);
    // std::cout << "Size: " << p.size() << "\n";
    
    // ERROR esperado: Esto no debe compilar bajo ninguna circunstancia
    // p.clear(); 
    return 0;
}
```

---

## Errores Comunes y Anti-Patrones
- **Usar herencia pública cuando se debió usar composición:** Por ejemplo, hacer que `class Cuadrado` herede de `class Rectangulo` públicamente. Esto viola las suposiciones de comportamiento del cliente (Liskov Substitution Principle).
- **Abusar de herencia privada por pereza:** Heredar privadamente de una clase gigante solo por reutilizar un método simple en lugar de delegar el trabajo mediante composición limpia.

---

## Conclusión y Checklist Mental
- [ ] ¿Cuál es la diferencia conceptual entre "tiene-un" y "se implementa en términos de"?
- [ ] ¿Qué beneficio en bytes de memoria ofrece la optimización EBO?
- [ ] ¿Por qué no puedes convertir un puntero derivado privado al tipo de la base en código cliente?

---

*Siguiente tema sugerido: [15 — Principios SOLID y Liskov uno por uno](<15 — Principios SOLID y Liskov uno por uno.md>)*


