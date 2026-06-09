# Idiosincrasias del C++ — Índice Temático Completo

El diseño de C++ expone el control físico de la máquina al programador, sacrificando la simplicidad por un rendimiento matemático sin abstracciones opacas. Este módulo de estudio aborda las complejidades y reglas subyacentes en la gestión del ciclo de vida, la memoria y la flexibilidad de tipos.

Dado lo masivo y profundo del tema, el contenido ha sido segmentado rigurosamente en cuatro submódulos exhaustivos.

---

## Módulos de Estudio

### [1. Tipos, Modificadores y Uniones](/Examen-III/02_A_Tipos_Modificadores_y_Uniones.md)
Detalla las reglas de inmutabilidad constante frente a macros inseguras. Profundiza en las restricciones críticas impuestas sobre las estructuras físicas de las uniones en C++ y las ventajas de los tipos suma modernos (`std::variant`).
- `const` vs `#define` y la transitividad de parámetros constantes.
- `enum` y la importancia semántica de la resolución `::`.
- El peligro del Type Punning en memoria compartida y reglas de herencia inhabilitadas.

### [2. Memoria y Ciclo de Vida](/Examen-III/02_B_Ciclo_de_Vida_y_Memoria.md)
Explica la secuencia de eventos entre el stack y el heap desde el inicio del programa global hasta la destrucción final local de los objetos.
- Lazy initialization por unidades de traducción (El desastre estático) y Linkage (`extern`).
- Constructor de Listas, Invocación Profunda (Depth-First).
- Copia vs Movimiento, Punteros Inteligentes y Destructores explícitos (Placement New).

### [3. Polimorfismo Físico](/Examen-III/02_C_Polimorfismo_Fisico.md)
Desmitifica cómo el compilador adultera físicamente a las clases abstractas para lograr el despacho dinámico en runtime.
- La Tabla Virtual (`vtable`) inyectada en solo-lectura y el Puntero Oculto (`vptr`).
- Penalizaciones de cache por el salto de memoria indirecto (Indirect Jump).
- La regla invariable de los destructores virtuales en clases base.

### [4. Sobrecarga de Funciones y Operadores](/Examen-III/02_D_Sobrecarga_Funciones_y_Operadores.md)
Trata cómo el polimorfismo estático flexibiliza las estructuras matemáticas con el costo de rigidez en firmas de compilación.
- Resolución de Ambigüedades y Name Mangling (El tipo de retorno no es firma).
- Operadores Miembro vs Funciones Globales y las cautelas extremas del `operator=`.
- Sobrecritura agresiva del Runtime Kernel con Pools propios en `operator new` y `operator delete`.
