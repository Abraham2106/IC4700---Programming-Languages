# Idiosincrasias del C++ — Gestión de Memoria, Ciclo de Vida y Polimorfismo Físico

C++ es un lenguaje que no oculta la arquitectura de la máquina; expone la memoria y el ciclo de vida de los objetos al programador. Estas idiosincrasias requieren un modelo mental riguroso donde cada decisión de diseño afecta el rendimiento y la seguridad.

---

## 1. Const-Correctness y Punteros

El uso de `const` en C++ no es solo una sugerencia, es un contrato verificado estáticamente que previene la mutación accidental de estado y facilita optimizaciones del compilador.

### 1.1 La Regla de Derecha a Izquierda

La lectura de declaraciones complejas se hace de derecha a izquierda alrededor del asterisco (`*`) para diferenciar la inmutabilidad del puntero frente a la inmutabilidad de los datos apuntados.

```cpp
int x = 10;
const int* ptr_a_const = &x; // El valor apuntado es inmutable.
int* const const_ptr = &x;   // El puntero en sí es inmutable, pero el valor puede cambiar.
const int* const ambos = &x; // Ni el puntero ni el valor pueden modificarse.
```

Un método constante (`void hacer_algo() const`) altera la firma de la función garantizando que los miembros del objeto no serán modificados (transformando el puntero oculto `this` de `T* const` a `const T* const`). Esto permite que instancias declaradas como constantes puedan invocar el método de forma segura ([02 — Const en Variables y Funciones.md](/C++/02_-_Const_en_Variables_y_Funciones.md)).

---

## 2. Gestión de Memoria y Ciclo de Vida

La dicotomía entre la pila (stack) y el montículo (heap) dicta cómo y cuándo se construye y destruye la memoria, marcando la diferencia entre el determinismo seguro y la gestión manual propensa a errores.

### 2.1 Stack vs Heap

Los objetos en la pila tienen un ciclo de vida atado a su ámbito (scope); se destruyen automáticamente al salir del bloque. Los objetos en el heap (`new`) sobreviven hasta que el programador invoca explícitamente `delete`. 

El olvido de esta invocación produce fugas de memoria, mientras que liberar la misma dirección dos veces corrompe el heap. La falta de recolector de basura obliga al uso de RAII (Resource Acquisition Is Initialization) para envolver punteros del heap en objetos de la pila, garantizando su limpieza determinista ([19 — Gestión de Memoria Stack vs Heap.md](/C++/19_-_Gestión_de_Memoria_Stack_vs_Heap.md)).

### 2.2 Anatomía de la Construcción y Destrucción

El order de construcción es estrictamente de arriba hacia abajo: primero la clase base, luego los miembros en su orden de declaración física, y finalmente el cuerpo del constructor. La destrucción ocurre exactamente a la inversa.

```cpp
class Base {
public:
    virtual ~Base() { std::cout << "Base dest\n"; }
};

class Derivada : public Base {
public:
    ~Derivada() override { std::cout << "Derivada dest\n"; }
};

// ...
Base* ptr = new Derivada();
delete ptr; 
```

Si el destructor de `Base` no fuera virtual, `delete ptr` solo invocaría el destructor de `Base`, dejando intactos los recursos alojados por `Derivada` e induciendo una fuga de memoria silenciosa. El polimorfismo dinámico exige destructores virtuales ([23 — Destructores (Invocacion directa de destructores Explicita e Implicita, definicion de operadores).md](/C++/23_-_Destructores_(Invocacion_directa_de_destructores_Explicita_e_Implicita,_definicion_de_operadores).md)).

---

## 3. Visibilidad y Sobrecarga de Operadores

El modelo de C++ permite extender la sintaxis del lenguaje a los tipos definidos por el usuario, proporcionando una expresividad equivalente a los tipos primitivos.

### 3.1 Estructuras y Clases

A nivel interno, un `struct` y una `class` son idénticos; la única diferencia semántica es la visibilidad por defecto. Los miembros de un `struct` (y su herencia) son públicos por defecto, mientras que en una `class` son privados. Las uniones, por otro lado, almacenan todos sus campos en la misma dirección de memoria, obligando a rastrear el tipo activo manualmente.

### 3.2 La Regla de Tres y el Operador de Asignación

Cuando una clase gestiona memoria manualmente, el constructor de copia, el operador de asignación (`operator=`) y el destructor deben implementarse explícitamente para evitar copias superficiales (shallow copies) que llevarían a la doble liberación.

```cpp
class Gestor {
    int* data;
public:
    Gestor& operator=(const Gestor& otro) {
        if (this == &otro) return *this; // Protección contra auto-asignación
        delete data;
        data = new int(*otro.data);
        return *this;
    }
};
```

El operador de asignación debe verificar siempre la auto-asignación (`x = x`) antes de liberar la memoria actual, o destruirá los datos que pretendía copiar ([25 — Asignacion con Clases y operator=.md](/C++/25_-_Asignacion_con_Clases_y_operator=.md)). Operadores asimétricos o binarios que requieren promoción de tipos en el argumento izquierdo deben ser implementados como funciones no miembro (globales), frecuentemente marcadas como `friend` para acceder al estado interno.

---

## 4. Inicialización Global y el Desastre Estático

Los objetos globales se inicializan antes de que inicie `main()`, pero C++ no garantiza el orden de inicialización entre diferentes unidades de traducción (archivos `.cpp`).

Si un objeto global en `A.cpp` depende de un objeto global en `B.cpp`, es posible que el segundo no haya sido construido todavía al momento de la ejecución. Este problema se esquiva con técnicas como el patrón "Construct on First Use" o variables estáticas dummy que fuerzan la carga determinista (como hace la biblioteca iostreams con `std::cout`).
