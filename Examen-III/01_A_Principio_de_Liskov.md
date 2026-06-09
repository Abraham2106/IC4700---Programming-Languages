# Principio de Sustitución de Liskov (LSP) y Principios SOLID

El diseño de software orientado a objetos en lenguajes fuertemente tipados como C++ depende críticamente del contrato establecido entre las clases base y sus derivadas. El **Principio de Sustitución de Liskov (LSP)** formaliza matemáticamente qué significa que una clase "herede" de otra, asegurando que el polimorfismo dinámico no induzca errores en tiempo de ejecución.

---

## 1. El Marco Teórico: Los Principios SOLID

LSP es la "L" en los principios SOLID, los cuales rigen el diseño arquitectónico estructurado:

1. **S**ingle Responsibility (Responsabilidad Única): Una clase debe tener exactamente una razón para cambiar (ej. separar lógica de datos de la persistencia).
2. **O**pen/Closed (Abierto/Cerrado): El software debe estar abierto a la extensión (mediante herencia y polimorfismo) pero cerrado a la modificación (sin alterar la clase base).
3. **L**iskov Substitution (Sustitución de Liskov): El núcleo del sub-tipado seguro.
4. **I**nterface Segregation (Segregación de Interfaces): Es preferible tener múltiples interfaces modulares que una "interfaz gorda" monolítica.
5. **D**ependency Inversion (Inversión de Dependencias): Depender siempre de abstracciones (punteros a clases base `std::unique_ptr<Base>`), no de implementaciones concretas.

---

## 2. Definición Formal de Liskov

> *"Si $S$ es un subtipo de $T$, entonces los objetos de tipo $T$ en un programa pueden ser reemplazados por objetos de tipo $S$ sin alterar ninguna de las propiedades deseables (correctitud) del programa."*

En términos prácticos, Liskov establece un **Contrato de Diseño por Contrato (Design by Contract)**. La herencia sintáctica (escribir `class S : public T`) no es suficiente; debe existir una herencia *semántica*.

### 2.1 Reglas del Contrato

Para que $S$ sea un subtipo válido de $T$, la clase $S$ está sometida a restricciones rigurosas:
1. **Precondiciones:** La clase derivada $S$ **no puede requerir más** que la clase base $T$. Si $T$ acepta cualquier entero positivo, $S$ no puede repentinamente lanzar una excepción si recibe el número 0.
2. **Postcondiciones:** La clase derivada $S$ **no puede entregar menos** que la base $T$. Si $T$ garantiza devolver una conexión activa, $S$ no puede devolver un puntero nulo silencioso.
3. **Invariantes:** $S$ debe preservar todos los estados lógicos que $T$ prometió mantener intactos.
4. **Excepciones:** $S$ no puede arrojar nuevos tipos de excepciones que el cliente de $T$ no esté preparado para capturar.

---

## 3. Violaciones Clásicas del LSP

La violación de Liskov suele presentarse cuando forzamos a una clase a heredar de otra solo por conveniencia de reutilización de código (Herencia Sintáctica), ignorando el comportamiento esperado (Herencia Semántica).

### 3.1 El Anti-Patrón del Cuadrado y el Rectángulo

Matemáticamente, un cuadrado "es un" rectángulo. Sin embargo, en la POO, heredar `Cuadrado` de `Rectangulo` rompe el contrato invariante si los lados son mutables.

```cpp
class Rectangulo {
protected:
    int ancho, alto;
public:
    virtual void set_ancho(int a) { ancho = a; }
    virtual void set_alto(int a) { alto = a; }
    int get_area() const { return ancho * alto; }
};

class Cuadrado : public Rectangulo {
public:
    // Un cuadrado obliga a que el ancho y el alto muten juntos.
    void set_ancho(int a) override { ancho = alto = a; }
    void set_alto(int a) override { ancho = alto = a; }
};
```

**La Ruptura del Contrato:**
Un cliente (una función genérica) que recibe un `Rectangulo& r` asumirá razonablemente que al invocar `r.set_ancho(5)` y `r.set_alto(10)`, el área resultante será de 50. Si pasamos un `Cuadrado`, el área sorpresivamente será 100, violando la expectativa de independencia de los ejes del rectángulo. El programa pierde la corrección matemática.

### 3.2 El Anti-Patrón de Listas Restringidas

Suponga una base `VectorBase` que permite añadir cualquier entero a un arreglo. Si creamos una clase derivada `VectorSoloPositivos` que lanza una excepción al intentar añadir números negativos, hemos violado Liskov. La derivada ha **endurecido las precondiciones**. Un cliente ciego operando sobre un puntero `VectorBase*` crasheará inexplicablemente al pasarle un número negativo, ya que la base le prometía aceptar la inserción.

### 3.3 El Olor del Código (Code Smell)

La violación de LSP se hace evidente arquitectónicamente cuando el programador cliente se ve obligado a utilizar *downcasting* dinámico o validación de tipos explícita (el equivalente a sentencias `if(type == ...)` o `dynamic_cast` en C++) para parchear el comportamiento irregular de la subclase. Si necesitas saber quién es el objeto para saber cómo tratarlo, el polimorfismo ha fracasado.
