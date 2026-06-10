# Idiosincrasias del C++ — Memoria y Ciclo de Vida

En C++, el ciclo de vida de los objetos expone la arquitectura real de la máquina. La memoria se administra en la pila (stack) o en el montículo (heap), y la inicialización está regida por reglas que priorizan el rendimiento físico sobre la abstracción.

---

## 1. Inicialización Global y Enlace (Linkage)

Los objetos globales o estáticos tienen un ciclo de vida que abarca toda la ejecución del programa, pero su orden de inicialización encierra sutilezas críticas.

### 1.1 El Desastre de la Inicialización Global

Se suele decir erróneamente que las variables globales se inicializan antes de que comience la función `main()`. La especificación real de C++ es más pragmática: garantiza que se inicializan antes de su **primer uso**. El compilador tiene la libertad de inicializarlas antes del `main()` o aplazarlo mediante una carga perezosa (lazy initialization) por cada unidad de traducción (archivo `.cpp`).

Esta falta de un orden determinista estricto entre diferentes archivos provoca el "Static Initialization Order Fiasco". Si un objeto global en `A.cpp` requiere de un objeto global en `B.cpp` en su constructor, el programa podría colapsar si `B` no ha sido construido. Esto se remedia con el patrón "Construct on First Use" (encapsular el global dentro de una función como estático local).

### 1.2 Linkage y la Palabra Clave `extern`

Para compartir una variable global entre múltiples archivos, no basta con declararla en un `.h`. Si se inicializa en el cabezal, la directiva del preprocesador copiará la inicialización a múltiples `.cpp`, generando un error de enlazador (Linker Error) por violar la Regla de Definición Única (ODR).

La palabra clave `extern` permite "declarar sin definir", prometiéndole al compilador que la variable física reside en otra unidad de traducción.

---

## 2. Construcción Profunda (Depth-First)

El proceso de construir un objeto compuesto (con herencia y atributos) no es un evento plano, sino un árbol recorrido en profundidad estricta.

### 2.1 Orden Exacto de Nacimiento

Cuando se instancia un objeto derivado, C++ ignora el orden en que se asignen los atributos dentro del código y sigue las reglas físicas de la estructura:
1. Llamada recursiva a constructores de la clase base (evaluando de izquierda a derecha en casos de herencia múltiple).
2. Construcción de los miembros de la clase en el orden **exacto** en que fueron declarados en la clase, no en el orden de la lista del constructor.
3. Ejecución del cuerpo de código dentro de las llaves del constructor actual.

Si el constructor de `Derivada` inicializa un miembro `y` usando el miembro `x` antes de que `x` sea construido, leerá basura estática o provocará un fallo de segmentación.

### 2.2 Listas de Inicialización de Miembros

Es un error de diseño inicializar clases complejas asignándoles un valor dentro del cuerpo del constructor (`this->str = "hola";`). El compilador primero inicializará `str` por defecto y luego reasignará el valor, duplicando el trabajo. Las listas de inicialización (`MiClase() : str("hola") {}`) construyen directamente el miembro en la memoria, siendo el único modo válido de inicializar referencias y constantes.

---

## 3. Variantes de Construcción y Semánticas

El lenguaje provee constructores para abarcar cualquier escenario de ciclo de vida, desde la prohibición de instanciar hasta la reubicación de memoria pura.

### 3.1 Constructores Privados y Clases Abstractas

Un constructor `private` o `protected` prohíbe que el cliente instancie el objeto en la pila directamente. Es la base para patrones de diseño como el Singleton o el Factory. De manera similar, declarar una función virtual pura (`virtual void f() = 0;`) convierte a la clase en abstracta: no se permite invocar ningún constructor de forma aislada, aunque deben existir para que las subclases los invoquen internamente.

### 3.2 Constructor por Copia vs Movimiento

El constructor de copia (`T(const T&)`) duplica los recursos de un objeto hacia uno nuevo realizando una asignación bit a bit (o recursiva profunda si se sobreescribe). En el caso de buffers grandes en el Heap, esta copia es extremadamente lenta.

El constructor de movimiento (`T(T&&)`) es una adición crítica de C++11. En lugar de duplicar memoria, "roba" el puntero al Heap del objeto temporal (rvalue) original y anula el origen. Al evitar las llamadas de asignación profunda del SO, transforma una copia $O(n)$ en un intercambio $O(1)$.

### 3.3 Instancias Anónimas

Los objetos creados en el sitio como valores temporales (`procesar(Vector(1, 2));`) no tienen nombre. Su vida útil es el ámbito exacto de la expresión (Statement Scope). Una vez la línea finaliza con un punto y coma, el objeto anónimo se destruye para limpiar la pila de forma predecible.

---

## 4. Destrucción e Invocación Directa

El proceso de destrucción es el inverso simétrico perfecto del orden de construcción, de abajo hacia arriba. Esto aplica incluso a las variables globales: se destruyen en orden inverso al que lograron ser inicializadas a lo largo de las distintas unidades de traducción.

### 4.1 Invocación Directa y Placement New

En entornos críticos, C++ permite reservar una losa de bytes puros en la memoria e "instanciar" físicamente el objeto sobre esa memoria pre-asignada usando *placement new* (`new (ptr) Clase()`). 

Dado que no se solicitó memoria dinámica formalmente para el sistema operativo mediante el gestor general, sería desastroso utilizar el comando `delete ptr;` tradicional. Para limpiar estos objetos, se debe realizar la única invocación explícita válida del destructor: `ptr->~Clase();`.

### 4.2 Punteros Inteligentes (Smart Pointers)

Para erradicar la necesidad del comando destructivo manual `delete`, la biblioteca estándar introdujo templates de gestión RAII (`std::unique_ptr<T>`, `std::shared_ptr<T>`).

Un smart pointer es una clase de la pila que encapsula un puntero desnudo. Al salir de su ámbito y ser destruida por las reglas estándar, su propio destructor se encarga de invocar a `delete` sobre el puntero encapsulado internamente, logrando recolección de basura predecible (sin la latencia de un Garbage Collector tradicional).

---

## 5. Inicialización vs Asignación — Constructor vs `operator=` (Examen P2)

Esta es una de las fuentes de confusión más comunes en C++. La regla es simple pero absoluta:

> **`=` en una declaración** → llama al **constructor** (el objeto aún no existe).
> **`=` sobre un objeto ya construido** → llama a **`operator=`** (el objeto ya existe).

### 5.1 Ejemplo canónico

```cpp
class A {
    int n;
public:
    A(int a) : n(a) {}              // constructor con argumento
    A& operator=(int a) { n = a; return *this; }  // operador de asignación
};

A a(3);   // (1) Constructor A(int)       — inicialización directa
A b = 5;  // (2) Constructor A(int)       — inicialización por copia (NO operator=)
a = 7;    // (3) operator=(int)           — asignación sobre objeto existente
```

| Línea | Qué llama | Justificación |
|---|---|---|
| `A a(3);` | **Constructor** `A(int)` | Sintaxis de inicialización directa. El objeto `a` se crea con `n=3`. |
| `A b = 5;` | **Constructor** `A(int)` | `b` aún no existe. El `=` aquí es **inicialización**, no asignación. El compilador convierte `5` a `A` usando el constructor. Si el constructor fuera `explicit`, **no compilaría**. |
| `a = 7;` | **`operator=(int)`** | `a` ya está construido. El `=` invoca el operador de asignación, sobreescribiendo `n = 7`. |

### 5.2 Constructor `explicit`

Si se marca el constructor como `explicit`, la inicialización implícita `A b = 5;` deja de funcionar porque el compilador no puede hacer la conversión automática:

```cpp
explicit A(int a) : n(a) {}

A b = 5;    //  Error: conversión implícita prohibida
A b(5);     //  OK: inicialización directa explícita
A b = A(5); //  OK: conversión explícita manual
```

---

## 6. Errores Comunes en Constructores y Herencia (Examen P4)

### 6.1 Los constructores NO tienen tipo de retorno

Un constructor es el único miembro de una clase que **no tiene tipo de retorno**, ni siquiera `void`. Agregar un tipo convierte la declaración en una función ordinaria con el mismo nombre que la clase, lo que produce un **error de compilación**:

```cpp
//  CÓDIGO CON ERRORES
class Foo {
    Foo& Foo(int i) { /* … */ }  // Error: tipo de retorno 'Foo&' inválido en constructor
};
class Bar : public Foo {
    Bar& Bar() { /* … */ }       // Error: tipo de retorno 'Bar&' inválido en constructor
};
```

Hay **tres problemas** en este código:

| # | Problema | Causa | Consecuencia |
|---|---|---|---|
| 1 | **Tipo de retorno en el constructor** | `Foo& Foo(int i)` y `Bar& Bar()` declaran un tipo de retorno | **Error de compilación** — los constructores no devuelven nada. |
| 2 | **Visibilidad `private` por defecto** | En `class`, todos los miembros son `private` si no se especifica `public:` | Si los constructores quedan privados, **no se pueden crear objetos** desde fuera de la clase. |
| 3 | **`Foo` no tiene constructor por defecto** | Solo se define `Foo(int i)`, eliminando el constructor por defecto implícito | Cuando `Bar` intenta construirse, el compilador busca `Foo()` (sin argumentos) y **no lo encuentra** → error de compilación. |

### 6.2 Versión corregida

```cpp
//  CÓDIGO CORRECTO
class Foo {
public:
    Foo(int i) { /* … */ }        // Sin tipo de retorno; público
};

class Bar : public Foo {
public:
    Bar() : Foo(0) { /* … */ }   // Inicializa la base explícitamente
};
```

**Reglas que aplica la corrección:**

1. **Sin tipo de retorno** en ningún constructor.
2. **`public:`** antes de los constructores para permitir instanciación desde fuera.
3. **Lista de inicialización `: Foo(0)`** en `Bar` para invocar el único constructor disponible de `Foo`. Alternativamente, `Foo` podría proveer un constructor por defecto `Foo() {}`.

### 6.3 Regla del Constructor por Defecto

> Si una clase define **cualquier constructor**, el compilador **deja de generar** el constructor por defecto implícito (`ClassName()`).
> La clase derivada **siempre** debe inicializar la base; si no lo hace explícitamente, el compilador busca el constructor por defecto de la base. Si no existe → **error de compilación**.
