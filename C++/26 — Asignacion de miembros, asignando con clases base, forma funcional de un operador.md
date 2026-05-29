# 26 — Asignación de Miembros y Clases Base

> **Resumen Ejecutivo:** Cuando una clase hereda de otra y sobrecarga su propio `operator=`, es responsable de invocar explícitamente el operador de asignación de su clase base para evitar dejar la porción heredada del objeto sin actualizar. Esta nota analiza las mecánicas de asignación miembro a miembro, el problema de rebanamiento (*slicing*) en accesos polimórficos y la sintaxis funcional para delegar asignaciones en jerarquías complejas.
>
> **Prerrequisitos:** Haber leído [12 — Herencia y Visibilidad](<12 — Herencia y Visibilidad.md>) y [25 — Asignación con Clases y operator=](<25 — Asignacion con Clases y operator=.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Asignación e Herencia](#teoría-asignación-e-herencia)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
La asignación de miembros (*member-wise assignment*) en una clase derivada debe extenderse a la clase base. Si la clase derivada define su propio operador de asignación, el compilador desactivará la generación automática de la parte base, transfiriendo al desarrollador la responsabilidad de delegar la asignación de forma explícita.

### ¿Por qué importa?
- **Pérdida de Consistencia:** Si omites la invocación de la base, el objeto destino quedará en un estado híbrido: sus campos locales tendrán valores nuevos, pero sus campos heredados mantendrán valores obsoletos.
- **Slicing Polimórfico:** Si asignas un objeto derivado a una referencia o variable de su clase base, el compilador recortará silenciosamente toda la porción derivada, copiando únicamente los atributos de la base.

---

## Conceptos Previos
- Upcasting de referencias (compatibilidad de derivada a base).
- Ocultamiento de funciones base (*shadowing*) por resolución de ámbito.
- Herencia múltiple y virtual en C++.

---

## Hook Example

```cpp
#include <iostream>
#include <string>

class ComponenteBase {
    std::string identificador;
public:
    ComponenteBase(std::string id) : identificador(id) {}

    ComponenteBase& operator=(const ComponenteBase& otro) {
        if (this != &otro) {
            identificador = otro.identificador;
            std::cout << "[BASE] Asignacion de ComponenteBase realizada.\n";
        }
        return *this;
    }

    void mostrarBase() const {
        std::cout << "ID Componente: " << identificador << "\n";
    }
};

class DispositivoElectronico : public ComponenteBase {
    int voltaje;
public:
    DispositivoElectronico(std::string id, int v) 
        : ComponenteBase(id), voltaje(v) {}

    // Sobrecarga de asignacion de la clase derivada
    DispositivoElectronico& operator=(const DispositivoElectronico& otro) {
        if (this != &otro) {
            // 1. Invocar explicitamente el operador de la clase base
            // sintaxis completamente calificada para evitar recursión infinita
            this->ComponenteBase::operator=(otro); 

            // 2. Asignar atributos locales de la derivada
            voltaje = otro.voltaje;
            std::cout << "[DERIVADA] Asignacion de DispositivoElectronico realizada.\n";
        }
        return *this;
    }

    void mostrarCompleto() const {
        mostrarBase();
        std::cout << "Voltaje: " << voltaje << "V\n";
    }
};

int main() {
    DispositivoElectronico d1("Sensor_A", 5);
    DispositivoElectronico d2("Sensor_B", 12);

    std::cout << "--- Realizando asignacion derivada ---\n";
    d2 = d1; 
    d2.mostrarCompleto();

    return 0;
}
```

---

## Descomposición Under the Hood

### ¿Por qué se requiere la sintaxis `this->Base::operator=(otro)`?
En C++, las funciones en el ámbito de una clase derivada ocultan (*shadow*) a las funciones que comparten el mismo nombre en la clase base.
- El operador de asignación `operator=` no es una excepción. El `operator=` de `DispositivoElectronico` oculta al `operator=` de `ComponenteBase`.
- Si dentro del operador de la derivada intentas escribir `operator=(otro);`, el compilador interpretará que estás llamando recursivamente a la función del operador de la clase derivada actual. Esto resultará en un desbordamiento inmediato de la pila de llamadas (*stack overflow*).
- Calificar la llamada con el nombre de la clase base (`Base::operator=`) rompe el proceso de búsqueda del compilador y redirige la llamada directamente al método de la base. El parámetro `otro` (tipo `DispositivoElectronico`) se promociona de forma segura a una referencia `const ComponenteBase&` de manera implícita.

---

## Teoría: Asignación e Herencia

### El problema de Slicing (Rebanamiento)
Ocurre cuando asignamos por valor un objeto de la clase derivada a un objeto de la clase base:

```cpp
DispositivoElectronico disp("Sensor", 5);
ComponenteBase comp("Generico");

comp = disp; // ¡SLICING!
```

En memoria, lo que sucede es:

```
OBJETO DERIVADO (disp)                  OBJETO BASE (comp)
┌────────────────────────┐              ┌────────────────────────┐
│ [ComponenteBase]       │              │ [ComponenteBase]       │
│   identificador: Sensor│─────────────>│   identificador: Sensor│
├────────────────────────┤              └────────────────────────┘
│ [DispositivoElectronico]│ (Descartado)
│   voltaje: 5V          │
└────────────────────────┘
```
El objeto `comp` no tiene espacio físico para almacenar el atributo `voltaje`. Por lo tanto, el compilador solo copia la porción correspondiente a `ComponenteBase`. El comportamiento dinámico polimórfico del objeto asignado se pierde por completo.

---

## Progresión de Complejidad

### Nivel Simple: Asignación automática por defecto con herencia
Si la clase derivada **no** declara un operador de asignación, el compilador genera uno automático que invoca de forma recursiva al operador de asignación de su clase base y luego realiza la asignación miembro a miembro de todos los campos locales de la clase derivada.

### Nivel Aplicado: Protección del Slicing mediante herencia protegida
Para impedir que los programadores sufran de slicing accidental al trabajar con jerarquías de clases polimórficas, es recomendable declarar el operador de asignación de la clase base polimórfica como `protected`.
- Esto permite que las clases derivadas sigan invocando el operador base para su lógica interna.
- Impide que el código cliente externo pueda realizar asignaciones directas del tipo `ObjetoBase = ObjetoDerivado`, forzando al compilador a detenerse con un error de acceso privado.

### Nivel Complejo: Asignación y herencia virtual
Si una clase hereda de forma múltiple a través de un ancestro común virtual (ej. Jerarquía de Diamante de C++), la clase base común virtual se inicializa y comparte una única vez en memoria física.
- Al escribir operadores de asignación manuales en este escenario, la base virtual puede acabar siendo asignada múltiples veces por diferentes ramas de la jerarquía.
- Se requiere estructurar los operadores de asignación para que deleguen la porción virtual solo una vez, o usar funciones de soporte protegidas (`void assignBase(...)`) libres de efectos colaterales de re-asignación en las ramas intermedias.

---

## Diseño de Sistemas
En arquitectura de sistemas orientados a objetos polimórficos, muchas veces se opta por clonación dinámica mediante un método virtual puro `virtual Base* clone() const = 0;` en lugar de soportar asignación por operadores directos, ya que la asignación directa sobre referencias polimórficas no puede ser resuelta de forma dinámica nativa.

---

## Ejercicios

### Ejercicio 1 — Simular pérdida de datos por falta de delegación
En la clase `DispositivoElectronico` del `Hook Example`, elimina la línea `this->ComponenteBase::operator=(otro);`. Ejecuta el código e imprime el estado final del objeto `d2`. ¿Por qué su identificador no cambió a pesar de haberse ejecutado la asignación?

---

## Errores Comunes y Anti-Patrones
- **Intentar hacer casts de punteros como l-values:** Escribir `((ComponenteBase)*this) = otro;` crea un objeto temporal que se asigna y descarta, dejando el objeto original sin cambios en su sección base.
- **Ignorar el slicing polimórfico al almacenar colecciones:** Guardar objetos por valor en vectores de clases base (`std::vector<ComponenteBase>`) causa rebanamientos silenciosos de todas las clases derivadas añadidas. Almacena siempre punteros inteligentes (`std::vector<std::unique_ptr<ComponenteBase>>`).

---

## Conclusión y Checklist Mental
- [ ] ¿Cómo puedes invocar al operador de asignación de la clase base dentro del operador derivado?
- [ ] ¿Qué es el *slicing* de objetos y cómo se evita en C++?
- [ ] ¿Por qué un `operator=` derivado oculta (shadow) al operador base?

---

*Siguiente tema sugerido: [27 — Overloading Non-Member y Operadores de Conversión](<27 — Overloading Non-Member y Operadores de Conversion.md>)*
