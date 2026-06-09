# Asignación con Clases y operator= — Regla de tres/cinco y prevención de autoasignación en operator=

La sobrecarga de `operator=` requiere gestionar la liberación de recursos anteriores y la protección contra autoasignación. Implementar este operador de forma segura es indispensable para cumplir con la Regla de Tres y garantizar la consistencia en copias profundas.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
El operador de asignación sobrecargado reemplaza el comportamiento predeterminado de copia bit a bit provisto por el compilador. Este operador se encarga de:
1. Validar la autoasignación (`this == &other`).
2. Liberar los recursos de memoria del buffer existente.
3. Copiar profundamente los datos del objeto de la derecha.
4. Retornar `*this` para permitir la sintaxis `a = b = c`.

### 1.2 ¿Por qué importa?
- **Peligro de Autodestrucción:** Si un cliente ejecuta `x = x` y el operador de asignación no tiene un resguardo de autoasignación, el primer paso borrará el buffer de `x`. Dado que el objeto origen y destino son el mismo, el paso de copia intentará leer desde la memoria que acaba de ser borrada, provocando un fallo catastrófico de lectura de memoria liberada.

---

## 2. Hook Example

```cpp
#include <iostream>
#include <cstring>
#include <algorithm>

class BufferSeguro {
    char* datos;
    size_t tamano;
public:
    explicit BufferSeguro(const char* txt) {
        tamano = std::strlen(txt);
        datos = new char[tamano + 1];
        std::strcpy(datos, txt);
    }

    ~BufferSeguro() {
        delete[] datos;
    }

    // Constructor de copia necesario para copy-and-swap
    BufferSeguro(const BufferSeguro& otro) {
        tamano = otro.tamano;
        datos = new char[tamano + 1];
        std::strcpy(datos, otro.datos);
    }

    // Función swap no-throwing para intercambiar recursos
    void swap(BufferSeguro& otro) noexcept {
        std::swap(datos, otro.datos);
        std::swap(tamano, otro.tamano);
    }

    // Sobrecarga de asignacion clasica con guardia y borrado explicito
    BufferSeguro& operator=(const BufferSeguro& otro) {
        // 1. Validar autoasignacion (Self-assignment guard)
        if (this == &otro) {
            return *this;
        }

        // 2. Limpiar buffer existente (Clean buffer)
        delete[] datos;

        // 3. Reservar memoria nueva y copiar datos
        // NOTA: Si 'new' lanza una excepcion bad_alloc aqui, el objeto ya perdio 
        // su memoria original (datos = dangling). El estado del objeto queda corrupto.
        tamano = otro.tamano;
        datos = new char[tamano + 1];
        std::strcpy(datos, otro.datos);

        // 4. Retornar *this para permitir encadenamiento
        return *this;
    }

    // Sobrecarga moderna usando el Patron Copy-and-Swap (Garantia Fuerte ante Excepciones)
    // NOTA: Para activarlo, el parametro se pasa por valor, creando una copia automatica.
    /*
    BufferSeguro& operator=(BufferSeguro copia) {
        this->swap(copia);
        return *this;
    } // Aqui 'copia' se destruye automaticamente liberando la memoria vieja.
    */

    void imprimir() const {
        std::cout << "Buffer: " << datos << " (Size: " << tamano << ")\n";
    }
};

int main() {
    BufferSeguro b1("Datos original");
    BufferSeguro b2("Reemplazo");

    b2 = b1; // Llama a operator=
    b2.imprimir();

    // Autoasignación
    b1 = b1; 
    b1.imprimir();

    return 0;
}
```

---

## 3. Descomposición Under the Hood

### 3.1 ¿Qué hace el compilador con la cascada de asignaciones?
Cuando el compilador evalúa la expresión `a = b = c`:
1. El operador de asignación agrupa de derecha a izquierda: `a = (b = c)`.
2. Primero evalúa `b.operator=(c)`. Esta llamada realiza la copia de los recursos de `c` en `b` y retorna una referencia a `b` (`b&`).
3. La llamada externa se convierte en `a.operator=(referencia_a_b)`.
- Si el tipo de retorno fuera `void` en lugar de una referencia a la clase, la expresión externa recibiría un parámetro vacío y fallaría la compilación de la cascada.

---

## 4. Teoría: Sobrecarga del operator=

### 4.1 1. La Guardia de Autoasignación
La comprobación `if (this == &other)` compara las direcciones físicas en memoria del objeto invocador (`this`) y del parámetro. Es obligatoria en asignaciones directas de punteros.

### 4.2 2. El peligro de las excepciones de memoria en asignaciones clásicas
En la implementación clásica de 4 pasos (verificar, borrar, reasignar, retornar):
- El destructor `delete[] datos` ocurre **antes** de reservar memoria con `new char[...]`.
- Si la llamada a `new` falla y lanza una excepción por falta de memoria (`std::bad_alloc`), la ejecución del operador se interrumpe inmediatamente y la excepción se propaga al cliente.
- El objeto queda en un estado corrupto: su puntero interno `datos` apunta a memoria ya liberada, y cualquier intento posterior de destruirlo o usarlo provocará accesos inválidos.

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Asignación por Defecto
El compilador sintetiza un `operator=` por defecto si la clase no define uno. Este operador asigna todos los campos miembro secuencialmente, llamando a los operadores de asignación respectivos de cada atributo. Si el atributo es de tipo primitivo (ej. `int`, `double`, punteros nativos), copia el valor físico (Shallow Copy).

### 5.2 Nivel Aplicado: Garantía Fuerte de Excepciones mediante Copy-and-Swap
Para solucionar el problema de corrupción por excepciones de memoria, se aplica el patrón *Copy-and-Swap*:
1. Se pasa el parámetro **por valor** en lugar de por referencia constante: `X& operator=(X otro)`. Esto hace que el compilador cree una copia temporal del objeto derecho automáticamente usando el constructor de copia.
2. Si la copia falla por falta de memoria, el error ocurre **antes** de modificar el objeto izquierdo. El objeto izquierdo no se altera en absoluto.
3. Si la copia es exitosa, se intercambian (*swap*) los punteros internos del objeto izquierdo con los del objeto temporal usando una función que no lance excepciones (`noexcept`).
4. Al salir de la función del operador, el objeto temporal (que ahora contiene el buffer antiguo del objeto izquierdo) se destruye automáticamente, liberando la memoria.

### 5.3 Nivel Complejo: Deshabilitar Asignación (`= delete`)
En clases que controlan recursos exclusivos del hardware (sockets, conexiones de base de datos, wrappers de archivos), la copia lógica no tiene sentido. Se deshabilita explícitamente la asignación para que el compilador reporte un error en tiempo de compilación si alguien intenta asignar los objetos:
```cpp
class CanalAudio {
public:
    CanalAudio& operator=(const CanalAudio&) = delete; // Prohibido copiar
};
```

---

## 6. Diseño de Sistemas
En el diseño de sistemas de alto rendimiento, el patrón de diseño *Move Assignment Operator* (`operator=(X&& otro)` de C++11) complementa al operador de copia, permitiendo Únicamente transferir la propiedad de los punteros en lugar de realizar duplicaciones costosas en el heap, reduciendo el coste temporal de asignación a cero.

---

## Exercises

### Exercise 1 — Analizar la falla de autoasignación sin guardia
Modifica la clase del `Hook Example` removiendo la línea `if (this == &otro) return *this;`. En la función `main()`, ejecuta `b1 = b1;` e intenta imprimir su estado. ¿Por qué el programa reporta caracteres extraños o falla?

---

## 7. Errores Comunes y Anti-Patrones
- **No retornar referencia (`X&`):** Retornar `void` o un objeto por valor `X` incurre en overhead de copia adicional y deshabilita el encadenamiento de asignaciones.
- **Liberar la memoria antes de validar la autoasignación:** Causa destrucción silenciosa de los datos del objeto.

---

## 8. Conclusión

---

---

*Next: `26 — Asignacion de miembros, asignando con clases base, forma funcional de un operador.md` — Slicing y encadenamiento de asignaciones.*
