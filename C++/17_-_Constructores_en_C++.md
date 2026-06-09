# Constructores en C++ — Listas de inicialización de miembros y constructores delegados

Los constructores son funciones miembro especiales que inicializan el estado interno de un objeto garantizando sus invariantes de clase. C++ permite delegación de constructores y listas de inicialización para optimizar la construcción y evitar inicializaciones por defecto redundantes.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
Un constructor es una función miembro que comparte el mismo nombre que su clase, no tiene tipo de retorno, y se invoca automáticamente al crear una instancia de la clase.

### 1.2 ¿Por qué importa?
- **Garantizar invariantes:** Un objeto nunca debe nacer con un estado inconsistente o memoria corrupta.
- **Evitar conversiones implícitas no deseadas:** Olvidar la palabra clave `explicit` en constructores de un solo argumento permite que el compilador realice conversiones implícitas silenciosas que desatan bugs lógicos difíciles de depurar.

---

## 2. Hook Example

```cpp
#include <iostream>

class VectorSimulado {
    int* datos;
    int capacidad;
public:
    // 1. Constructor Parametrizado (marcado explicit para evitar conversiones raras)
    explicit VectorSimulado(int cap) : capacidad(cap) {
        datos = new int[capacidad]{0};
        std::cout << "Vector creado con capacidad: " << capacidad << "\n";
    }
    
    ~VectorSimulado() {
        delete[] datos;
    }
};

void procesar_vector(const VectorSimulado& v) {}

int main() {
    VectorSimulado v1(10); // Legal
    // procesar_vector(5); // ERROR gracias a 'explicit': impide convertir el entero 5 en VectorSimulado implícitamente.
    return 0;
}
```

---

## 3. Descomposición Under the Hood

### 3.1 ¿Qué ocurre en memoria al llamar al constructor?
- El constructor **no** aloca la memoria del objeto en sí mismo:
  - Si el objeto se crea en el Stack, el puntero de pila ya se desplazó para reservar los bytes necesarios.
  - Si se crea en el Heap via `new`, el operador `new` primero reserva los bytes físicos y luego invoca al constructor pasándole la dirección del bloque en el parámetro implícito `this`.
- El constructor se encarga de rellenar esa memoria física inicializando los datos miembro uno a uno según el orden de declaración en la clase.

---

## 4. Teoría: Tipos de Constructores

### 4.1 1. Constructor por Defecto (Default Constructor)
- Se invoca sin argumentos: `Persona p;`
- Si **no** declaras ningún constructor, el compilador genera uno por defecto automáticamente (siempre que todos los miembros tengan constructores por defecto).
- Si declaras cualquier constructor parametrizado, el compilador **deja de generar** el constructor por defecto. Debes solicitarlo explícitamente si lo deseas: `Persona() = default;`.

### 4.2 2. Constructor Parametrizado
- Toma argumentos para personalizar el estado inicial del objeto.

### 4.3 3. Constructor de Conversión y la importancia de `explicit`
Cualquier constructor que pueda llamarse con un solo argumento actúa por defecto como un **constructor de conversión**.
- Permite hacer asignaciones implícitas del tipo del argumento al tipo de la clase.
- **Prevención:** Anteponer `explicit` obliga al programador a usar inicializaciones directas o casts explícitos, desactivando conversiones automáticas no deseadas.

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Lista de inicialización de miembros (Member Initializer List)
Se prefiere inicializar miembros usando la sintaxis `: miembro(valor)` en lugar de asignarlos dentro del cuerpo del constructor (`miembro = valor`). La primera inicializa directamente el miembro; la segunda primero lo inicializa por defecto y luego lo reasigna (incurriendo en doble costo).
```cpp
class Punto {
    int x;
    int y;
public:
    Punto(int px, int py) : x(px), y(py) {} // Eficiente y correcto
};
```

### 5.2 Nivel Aplicado: Constructores Delegados (Delegating Constructors) - C++11
Un constructor puede delegar la lógica de inicialización en otro constructor de la misma clase para evitar duplicar código.
```cpp
class Rectangulo {
    int ancho, alto;
public:
    Rectangulo(int w, int h) : ancho(w), alto(h) {}
    // Delega en el constructor principal:
    Rectangulo(int lado) : Rectangulo(lado, lado) {} 
};
```

### 5.3 Nivel Complejo: Constructores Heredados (Inherited Constructors) - C++11
Podemos importar en bloque los constructores de la clase base a la derivada usando la directiva `using`.
```cpp
class Base {
public:
    Base(int x) {}
    Base(double y) {}
};

class Derivada : public Base {
public:
    // Importa automáticamente ambas sobrecargas del constructor
    using Base::Base; 
};
```

---

## 6. Diseño de Sistemas
En el diseño de interfaces limpias, el principio de **"RAII Completo"** dicta que el constructor de una clase debe adquirir todos sus recursos o fallar lanzando una excepción si es imposible inicializarse correctamente, evitando dejar el objeto en un estado inestable o inservible en producción.

---

## Exercises

### Exercise 1 — Evitar Conversión Implícita con explicit
Identifica el error de conversión implícita en el siguiente código y corrígelo aplicando la directiva `explicit` adecuadamente.

```cpp
#include <iostream>

class EnvoltorioEntero {
    int valor;
public:
    // TODO: Hacer este constructor explicit para evitar conversiones implícitas
    EnvoltorioEntero(int v) : valor(v) {}
    int get() const { return valor; }
};

void mostrar(const EnvoltorioEntero& e) {
    std::cout << "Valor: " << e.get() << "\n";
}

int main() {
    EnvoltorioEntero e1(10);
    mostrar(e1);
    
    // TODO: Una vez que agregues 'explicit', esta línea dará error de compilación.
    // Comenta la línea y reemplázala por una construcción explícita: mostrar(EnvoltorioEntero(42));
    mostrar(42); 
    return 0;
}
```

---

## 7. Errores Comunes y Anti-Patrones
- **Llamar al constructor por defecto con paréntesis vacíos:** Escribir `Persona p();` no crea un objeto; en su lugar, el compilador lo interpreta como la declaración de una función llamada `p` que no toma argumentos y retorna una `Persona` (conocido como *Most Vexing Parse*). La sintaxis correcta es `Persona p;` o `Persona p{};`.

---

## 8. Conclusión

---

---

*Next: `18 — Construcción por Copia y Movimiento.md` — Semántica de copia y transferencia eficiente de recursos.*
