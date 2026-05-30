# Herencia y Visibilidad — Visibilidad de miembros y herencia pública, protegida y privada

La herencia define relaciones jerárquicas y reutilización de código entre clases mediante los niveles de visibilidad public, protected y private. Controlar el flujo de acceso de los miembros derivados es esencial para mantener la cohesión y las invariants de la clase base.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
- **Herencia:** Mecanismo mediante el cual una clase derivada adquiere las características (atributos y métodos) de una clase base.
- **Visibilidad (Especificadores de acceso):**
  - `public`: Accesible desde cualquier lugar del programa.
  - `protected`: Accesible solo dentro de la clase y sus descendientes (clases derivadas).
  - `private`: Accesible única y exclusivamente dentro de la propia clase.

### 1.2 ¿Por qué importa?
Diseñar incorrectamente los niveles de visibilidad rompe las reglas de encapsulamiento básico. Exponer variables miembro públicas en lugar de protegerlas (`private`) con getters/setters permite que código externo corrompa el estado interno del objeto.

---

## 2. Hook Example

```cpp
#include <iostream>

class Vehiculo {
protected:
    int velocidad_maxima = 120; // Visible para clases hijas, privada para el exterior
private:
    std::string nro_chasis = "XYZ999"; // Oculto para todos menos Vehiculo
};

class Auto : public Vehiculo {
public:
    void mostrar_velocidad() {
        std::cout << "Velocidad: " << velocidad_maxima << " km/h\n"; // Legal: protected miembro heredado
        // std::cout << nro_chasis; // ERROR: private de la clase base no es accesible aquí.
    }
};
```

---

## 3. Descomposición Under the Hood

### 3.1 Disposición de Memoria en Herencia Simple
- En C++, la herencia no introduce sobrecarga en runtime de forma inherente si no hay funciones virtuales.
- Al instanciar un objeto de clase derivada, el compilador distribuye la memoria de manera secuencial y contigua:
  1. Primero coloca los campos de datos pertenecientes a la **clase base**.
  2. Inmediatamente después coloca los campos propios de la **clase derivada**.
- El tamaño en bytes del objeto derivado es Únicamente la suma de los tamaños de los miembros de la base más los de la derivada (más padding).
- Un puntero a la clase derivada (`Auto*`) puede convertirse implícitamente a un puntero de la clase base (`Vehiculo*`) porque la dirección inicial del objeto es exactamente la misma.

---

## 4. Teoría: Modos de Herencia y Visibilidad

### 4.1 1. Especificadores de Acceso dentro de la Clase
- **`public`**: Interfaz pública del objeto.
- **`protected`**: Interfaz de extensión para clases derivadas.
- **`private`**: Detalles de implementación interna ocultos.

### 4.2 2. Modos de Herencia (Herencia Pública por defecto en Structs, Privada en Classes)
El calificador en la declaración de herencia (`class Derivada : [modo] Base`) define el límite máximo de visibilidad de los miembros heredados en la clase hija:

| Visibilidad Original en la Base | Heredado vía `public` | Heredado vía `protected` | Heredado vía `private` |
|---|---|---|---|
| `public` | `public` | `protected` | `private` |
| `protected` | `protected` | `protected` | `private` |
| `private` | Inaccesible | Inaccesible | Inaccesible |

*Nota: La herencia privada y protegida ocultan la relación "es-un" al código externo.*

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Herencia Pública básica
```cpp
class Animal {
public:
    void comer() {}
};

class Perro : public Animal {
public:
    void ladrar() {}
};
```

### 5.2 Nivel Aplicado: Amistad (`friend`)
C++ permite que funciones o clases ajenas accedan a miembros privados usando la directiva `friend`. Esto debe usarse con extrema precaución para no debilitar el encapsulamiento.
```cpp
class CuentaBancaria {
    double saldo = 500.0;
    
    // Le otorga acceso completo a la clase Auditor
    friend class Auditor; 
};

class Auditor {
public:
    void auditar(const CuentaBancaria& c) {
        std::cout << "Saldo auditado: " << c.saldo << "\n"; // Acceso legal a private saldo
    }
};
```

### 5.3 Nivel Complejo: Herencia Múltiple y el Problema del Diamante (Diamond Problem)
Si una clase deriva de dos clases que a su vez comparten la misma clase base, se producen duplicados de los atributos de la base raíz. Se soluciona mediante **Herencia Virtual**.
```cpp
#include <iostream>

class Dispositivo {
public:
    int id;
};

// Se utiliza herencia virtual para garantizar una única instancia de Dispositivo en memoria
class Impresora : virtual public Dispositivo {};
class Escaner : virtual public Dispositivo {};

class Multifuncional : public Impresora, public Escaner {
public:
    void set_id(int val) {
        id = val; // Legal y no ambiguo gracias a la herencia virtual
    }
};
```

---

## 6. Diseño de Sistemas
En arquitectura de software, la herencia múltiple descontrolada suele ser una fuente persistente de bugs complejos y acoplamiento dañino. Es una buena práctica de diseño preferir la **Composición sobre la Herencia** cuando sea posible (relación "tiene-un" en lugar de "es-un").

---

## Exercises

### Exercise 1 — Corregir Especificadores de Acceso
Modifica los especificadores de acceso del siguiente código para que compile y preserve las buenas prácticas de encapsulamiento.

```cpp
#include <iostream>

class Personaje {
    // TODO: Modificar para que 'salud' sea accesible por Mago, pero no en main()
    int salud = 100;
public:
    void recibir_danio(int cant) { salud -= cant; }
};

class Mago : public Personaje {
public:
    void curar() {
        // TODO: Poder aumentar 'salud' directamente aquí
        // salud += 20;
    }
};

int main() {
    Mago gandalf;
    gandalf.curar();
    
    // ERROR esperado: Esto no debería compilar bajo ninguna circunstancia
    // gandalf.salud = 500; 
    return 0;
}
```

---

## 7. Errores Comunes y Anti-Patrones
- **No declarar destructores virtuales en clases base:** Si destruyes un objeto de clase derivada mediante un puntero de la clase base, no se invocará el destructor de la clase derivada, causando fugas de recursos (veremos esto en detalle en el tema 22).
- **Abusar de herencia protegida o privada:** Enmascara relaciones de herencia legítimas y complica innecesariamente la jerarquía de llamadas y la legibilidad.

---

## 8. Conclusión

---

---

*Next: `13 — Uniones y std_variant.md` — Tipos suma seguros en memoria.*
