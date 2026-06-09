# Idiosincrasias del C++ — Sobrecarga de Funciones y Operadores

La sobrecarga en C++ no es un simple azúcar sintáctico; es la base del polimorfismo estático. Permite que funciones, operadores matemáticos y rutinas de gestión de memoria compartan el mismo identificador lógico pero actúen de manera distinta basándose en sus firmas de tipos, resolviéndose completamente en tiempo de compilación.

---

## 1. Sobrecarga de Funciones (Overloading)

A diferencia de C, donde cada función exige un identificador único global, C++ enmascara internamente el nombre de las funciones adjuntándole una codificación de sus parámetros (Name Mangling).

### 1.1 Resolución de Firmas y Ambigüedades

Para que dos funciones sobrecargadas sean válidas, el compilador debe poder distinguirlas basándose estrictamente en sus argumentos (tipo, número u orden).

- El tipo de retorno **no** forma parte de la firma lógica. Dos funciones que sólo difieren en lo que retornan causarán un error inmediato, ya que el compilador no sabría cuál invocar dependiendo del contexto de uso.
- La const-correctness **sí** es parte de la firma en métodos de clase. Una clase puede ofrecer un método `int& get()` para devolver una referencia modificable, y sobrecargarlo simultáneamente con `const int& get() const` para proteger los datos cuando el objeto anfitrión es constante.

Las conversiones implícitas (ej. pasar un `float` a una sobrecarga de `int`) pueden provocar ambigüedades si existen múltiples caminos de promoción posibles.

---

## 2. Sobrecarga de Operadores

C++ permite que las clases definidas por el usuario utilicen los operadores aritméticos (`+`, `-`, `*`), lógicos (`==`, `<`) y de acceso estructural (`[]`, `->`) manteniendo la precedencia y asociatividad estándar del lenguaje.

### 2.1 Miembro vs No-Miembro

Al sobrecargar un operador algebraico, hay dos aproximaciones fundamentales con implicancias semánticas:

- **Como función miembro:** El operador pertenece a la clase del argumento izquierdo. Útil para operadores asimétricos o de mutación como `operator=` o `operator+=`, donde el operando izquierdo es el que sufre la alteración estricta y se accede mediante el puntero `this`.
- **Como función global (no-miembro):** Es obligatoria para operaciones simétricas (como `A + B`) donde el compilador podría necesitar aplicar conversiones implícitas sobre el argumento izquierdo, o cuando la clase izquierda no nos pertenece (por ejemplo, al sobrecargar `<<` para `std::ostream`). Se suele declarar como `friend` dentro de la clase para permitirle acceder a los campos privados.

### 2.2 Operadores Críticos: Asignación y Conversión

El operador de asignación (`operator=`) es uno de los más peligrosos debido a la gestión de recursos. Debe siempre blindarse comprobando auto-asignaciones (`if (this == &otro)`) antes de destruir su propia memoria para ingerir la copia. Cuando la clase deriva de otra, el operador de asignación debe explícitamente invocar la asignación de su clase base (`Base::operator=(otro);`) o de lo contrario corromperá el segmento de herencia del objeto.

C++ también permite definir Operadores de Conversión de Tipo (`operator int() const`). Si no se marcan como `explicit`, el compilador usará estos operadores en silencio, pudiendo convertir un objeto complejo a un entero primitivo en medio de una suma, desatando bugs lógicos silenciosos.

### 2.3 Operadores Funcionales y de Acceso Estructural

La sobrecarga de `operator()` permite tratar instancias de una clase como si fueran llamadas a función (Functors). Conservan estado (como contadores o configuraciones internas) entre distintas ejecuciones, ventaja que los punteros a función primitivos carecen.

Sobrecargar el operador flecha (`operator->`) y desreferencia (`operator*`) es el núcleo para construir abstracciones como los *Smart Pointers* (e.g. `std::unique_ptr`), permitiéndoles imitar a un puntero rústico engañando al compilador con extrema eficiencia.

---

## 3. Sobrecarga de new y delete

La memoria dinámica global suele gestionarse a través del kernel del SO, lo que introduce alta latencia. C++ permite sobreescribir cómo se comporta el lenguaje a la hora de alocar bytes físicos sobreescribiendo `operator new` y `operator delete`.

### 3.1 Gestores de Memoria Personales (Custom Allocators)

Se puede definir un `void* operator new(size_t)` local en una clase para dictaminar de dónde provienen los bytes de esa instancia. En entornos de baja latencia o embebidos, se pre-asigna un bloque colosal de memoria ("Pool de memoria" o "Free List") y el operador sobrecargado simplemente reparte bloques contiguos de ese Pool en $O(1)$, eludiendo completamente las llamadas al sistema.

### 3.2 Placement New y Alineación (Alignment)

La forma funcional sobrecargada de asignación se conoce como Placement New (`new (puntero) Objeto()`). Aquí no hay alocación en absoluto; simplemente se fuerza la inicialización de los atributos dentro de un bloque pre-existente. 

Cualquier bloque cedido por una sobrecarga de `new` o usado en Placement New debe cumplir con un rigor de alineamiento de procesador estricto (`alignof(T)`). Un bloque de bits desalineado puede causar, dependiendo de la arquitectura de la CPU, desde latencias encadenadas severas hasta colapsos inmediatos de bus del hardware (Hardware Traps).
