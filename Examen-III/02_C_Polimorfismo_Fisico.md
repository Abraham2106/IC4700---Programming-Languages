# Idiosincrasias del C++ — Polimorfismo Físico

El polimorfismo dinámico en C++ permite que un bloque de código opere sobre objetos de múltiples subtipos heredados ignorando su verdadera forma estática, pero este poder no es gratuito. Para que el compilador separe el tipo declarado del tipo real de un objeto, deforma físicamente la memoria del programa.

---

## 1. El Despacho Dinámico y la Estructura Virtual

Cuando una clase C++ normal se compila, la llamada a cualquiera de sus métodos es resuelta por el enlazador estáticamente: un reemplazo de texto crudo que apunta a la dirección de ensamblador de la función. El polimorfismo rompe esta certeza y requiere mecanismos en tiempo de ejecución.

### 1.1 La Tabla Virtual (vtable)

El compilador inspecciona si una clase declara al menos una función con la palabra clave `virtual`. De ser así, genera silenciosamente una tabla estática en el segmento de solo lectura del ejecutable, conocida como `vtable`.

Esta tabla no es más que un arreglo de punteros a función. Cada entrada representa un método virtual de la clase, y apunta directamente al bloque de ensamblador de la implementación más derivada (o especializada) disponible para dicho método. Existe una `vtable` única e inmutable por cada tipo de clase en el sistema, compartida por todos los objetos de esa misma clase.

### 1.2 El Puntero Virtual (vptr)

Para conectar los objetos físicos en RAM con su correspondiente tabla virtual, el compilador adultera el tamaño real del objeto e inserta silenciosamente un puntero oculto (el `vptr`).

Cada vez que un cliente instancia un objeto de una clase polimórfica, los primeros 8 bytes físicos (en arquitecturas de 64 bits) corresponden a este puntero `vptr`, el cual es inicializado automáticamente por el constructor para apuntar a la base de la `vtable` de su propia clase.

```cpp
class EntidadPolimorfica {
    virtual void ejecutar() {}
    // El compilador inyecta: void** vptr;
};
```

---

## 2. El Costo de la Indirección

La maravilla del polimorfismo dinámico exige un costo de rendimiento que en C++ tradicionalmente se evita cuando no es estrictamente necesario, reflejando el principio "no pagas por lo que no usas".

### 2.1 Salto Indirecto (Indirect Jump)

Cuando se invoca un método virtual a través de un puntero base (`puntero_base->ejecutar();`), la CPU no puede saltar a un bloque de código estático.

1. La CPU primero debe leer el `vptr` oculto de la memoria del objeto apuntado.
2. Usando ese puntero, localiza la `vtable` física en el binario.
3. Lee la dirección de la función del array de la `vtable` sumando un índice estático precalculado.
4. Realiza un salto indirecto hacia esa última dirección.

Este proceso de doble indirección arruina predictores de salto de hardware y penaliza severamente la memoria caché, justificando por qué C++ no hace todos los métodos virtuales por defecto (a diferencia de Java).

---

## 3. Clases Abstractas y Destructores

El sistema virtual se extiende para garantizar seguridad de contratos de diseño puro o la limpieza inmaculada de clases extensas.

### 3.1 Funciones Virtuales Puras (= 0)

Una función virtual pura no provee implementación, forzando a que las clases derivadas tomen la batuta de responsabilidad. La simple existencia de una función virtual pura (ej. `virtual void calcular() = 0;`) desactiva el constructor de la clase, convirtiéndola matemáticamente en una clase abstracta ininstanciable.

La `vtable` de una clase abstracta deja un agujero nulo en la posición del puntero para esa función. Cualquier intento furtivo de invocar una función puramente virtual resulta en un error crítico (`pure virtual method called`), un crash de protección del sistema.

### 3.2 Destructores Virtuales: La Regla Inquebrantable

Si una clase se diseña para actuar como clase base bajo polimorfismo (es decir, el cliente manejará a la clase derivada usando un puntero de la base), su destructor **debe** declararse obligatoriamente como `virtual`.

Si el destructor de la base no es virtual, y se ejecuta el comando `delete ptr_base;`, el compilador emitirá un salto estático directo al destructor de la base. Los atributos propios de la clase derivada y todo el bloque de memoria que albergaban (sus arrays, sockets) nunca serán destruidos, garantizando una fuga de memoria letal. Declarar el destructor base como `virtual` inserta la limpieza final en la `vtable`, despachando el comando de destrucción hacia la derivada correspondiente primero.
