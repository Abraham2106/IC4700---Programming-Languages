# 15 — Principios SOLID y Liskov uno por uno

> **Resumen Ejecutivo:** Los principios SOLID son las cinco directrices de diseño orientadas a objetos fundamentales para escribir software mantenible, flexible y extensible. Este módulo los descompone de forma práctica en C++, prestando especial atención al Principio de Sustitución de Liskov (LSP).
>
> **Prerrequisitos:** Haber leído [12 — Herencia y Visibilidad](<12 — Herencia y Visibilidad.md>).
> **Clasificación:** TEMA MACRO

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Los Cinco Principios SOLID](#los-cinco-principios-solid)
  - [1. Single Responsibility Principle (SRP)](#1-single-responsibility-principle-srp)
  - [2. Open/Closed Principle (OCP)](#2-openclosed-principle-ocp)
  - [3. Liskov Substitution Principle (LSP)](#3-liskov-substitution-principle-lsp)
  - [4. Interface Segregation Principle (ISP)](#4-interface-segregation-principle-isp)
  - [5. Dependency Inversion Principle (DIP)](#5-dependency-inversion-principle-dip)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Ejercicios](#ejercicios)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué son los principios SOLID?
SOLID es un acrónimo acuñado por Robert C. Martin que describe cinco reglas de diseño estructural:
1. **S**ingle Responsibility (Responsabilidad Única)
2. **O**pen/Closed (Abierto/Cerrado)
3. **L**iskov Substitution (Sustitución de Liskov)
4. **I**nterface Segregation (Segregación de Interfaces)
5. **D**ependency Inversion (Inversión de Dependencias)

---

## Los Cinco Principios SOLID

### 1. Single Responsibility Principle (SRP)
*“Una clase debería tener una, y solo una, razón para cambiar.”*
- **Mal Diseño:** Una clase `Reporte` que contiene los datos del reporte, lo formatea a HTML, y además lo escribe en un archivo en el disco.
- **Buen Diseño en C++:** Separar en tres clases independientes: `Reporte` (datos), `FormateadorReporte` (formato) y `EscritorArchivo` (persistencia).

---

### 2. Open/Closed Principle (OCP)
*“Las entidades de software deben estar abiertas para su extensión, pero cerradas para su modificación.”*
- En C++, esto se logra principalmente mediante polimorfismo y clases abstractas. En lugar de escribir un bloque `switch` gigante que verifique el tipo de un objeto, se define una interfaz virtual común.

```cpp
class CanalEnvio {
public:
    virtual void enviar(const std::string& msg) = 0;
};

class EnviarSMS : public CanalEnvio {
public:
    void enviar(const std::string& msg) override { /* enviar SMS */ }
};

// Si mañana añadimos correo, no modificamos el código existente;
// simplemente creamos una nueva clase derivada EnviarCorreo : public CanalEnvio.
```

---

### 3. Liskov Substitution Principle (LSP)
*“Si S es un subtipo de T, los objetos de tipo T en un programa pueden ser reemplazados por objetos de tipo S sin alterar ninguna de las propiedades deseables del programa.”*

#### El Clásico Antipadrón: El Rectángulo y el Cuadrado
Sintácticamente, un Cuadrado es un Rectángulo (tiene 4 lados y ángulos de 90 grados). Sin embargo, si heredamos `Cuadrado : public Rectangulo` e intentamos redefinir `set_ancho` y `set_alto`:

```cpp
class Rectangulo {
protected:
    int ancho, alto;
public:
    virtual void set_ancho(int a) { ancho = a; }
    virtual void set_alto(int a) { alto = a; }
    int obtener_area() const { return ancho * alto; }
};

class Cuadrado : public Rectangulo {
public:
    void set_ancho(int a) override { ancho = alto = a; }
    void set_alto(int a) override { ancho = alto = a; }
};

// Una función cliente asume que puede alterar el ancho sin modificar el alto:
void redimensionar(Rectangulo& r) {
    r.set_ancho(5);
    r.set_alto(10);
    // Para un Rectangulo normal, el área debe ser 50.
    // Si r es un Cuadrado, ¡el área será 100! Esto viola el LSP y rompe la lógica del cliente.
}
```
**Solución:** No heredar si las precondiciones o postcondiciones difieren. Usar composición o una base común inmutable (ej. `FiguraGeometrica`).

---

### 4. Interface Segregation Principle (ISP)
*“Muchas interfaces específicas son mejores que una interfaz de propósito general.”*
- En C++, no declares clases base abstractas gigantescas ("interfaces gordas") que obliguen a las clases derivadas a implementar métodos que no necesitan (y a lanzar excepciones del tipo `NotImplemented`).

```cpp
// Malo: Interfaz gorda
class DispositivoMultifuncion {
public:
    virtual void imprimir() = 0;
    virtual void escanear() = 0;
    virtual void enviar_fax() = 0;
};

// Bueno: Interfaces segregadas
class Impresora { public: virtual void imprimir() = 0; };
class Escaner { public: virtual void escanear() = 0; };
```

---

### 5. Dependency Inversion Principle (DIP)
*“Depende de abstracciones, no de clases concretas.”*
- Los módulos de alto nivel no deben depender de los módulos de bajo nivel. Ambos deben depender de abstracciones.
- En C++, esto implica utilizar punteros o referencias a clases abstractas base (`std::unique_ptr<Interface>`) para inyectar dependencias en lugar de instanciar las implementaciones físicas directamente con `new`.

---

## Progresión de Complejidad

### Nivel Aplicado: Violación de LSP en Colecciones
Un error común es heredar públicamente para "reutilizar código" pero rompiendo los contratos de la clase base (ej. una lista que prohíbe inserciones de valores negativos).
```cpp
#include <vector>
#include <stdexcept>

class VectorBase {
protected:
    std::vector<int> items;
public:
    virtual void agregar(int x) { items.push_back(x); }
};

class VectorSoloPositivos : public VectorBase {
public:
    // Al sobreescribir, endurecemos las precondiciones, violando LSP.
    void agregar(int x) override {
        if (x < 0) throw std::invalid_argument("Solo positivos");
        items.push_back(x);
    }
};
```

---

## Diseño de Sistemas
En el diseño de frameworks desacoplados, aplicar Dependency Inversion (DIP) es clave para poder mockear (simular) servicios externos (como conexiones a bases de datos o APIs de pasarelas de pago) durante las pruebas unitarias.

---

## Ejercicios

### Ejercicio 1 — Identificar Principios Violados
Analiza el siguiente fragmento de código e identifica qué principios SOLID se están violando y cómo los corregirías.

```cpp
#include <iostream>

class GestorUsuarios {
public:
    // Guarda el usuario en la BD y además imprime logs a consola y envía emails
    void guardar_usuario(const std::string& username) {
        std::cout << "Guardando en BD el usuario: " << username << "\n";
        // Lógica de BD...
        
        std::cout << "LOG: Usuario guardado.\n";
        std::cout << "EMAIL: Bienvenido " << username << "\n";
    }
};

// TODO: Refactorizar la clase aplicando Single Responsibility Principle (SRP)
```

---

## Conclusión y Checklist Mental
- [ ] ¿Puedes enunciar los 5 principios de SOLID?
- [ ] ¿Qué significa violar el Principio de Sustitución de Liskov en la práctica?
- [ ] ¿Por qué el patrón de herencia Rectángulo/Cuadrado es conceptualmente incorrecto en OOP?

---

*Siguiente tema sugerido: [16 — Orden de Construcción y Destrucción (Depth)](<16 — Orden de Construcción y Destrucción (Depth).md>)*


