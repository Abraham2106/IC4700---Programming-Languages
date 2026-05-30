# Templates en C++ — Meta-programación genérica y especialización de código en tiempo de compilación

Los templates proporcionan el soporte nativo para la programación genérica y la metaprogramación en tiempo de compilación. Permiten generar especializaciones de clases y funciones eficientes que evitan la sobrecarga de tipos genéricos en runtime.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
Un template es una receta de código que le indica al compilador cómo generar una clase o función concreta sustituyendo los parámetros de plantilla (`template <typename T>`) por tipos de datos reales suministrados en el código del cliente.

### 1.2 ¿Por qué importa?
- **Seguridad Estática de Tipos:** En C tradicional, las colecciones genéricas usaban punteros `void*`. Esto obligaba a realizar casts manuales descendentes inseguros y propensos a provocar accesos inválidos en ejecución. Los templates permiten al compilador validar la consistencia de los tipos en compilación.
- **Evitar la Duplicación de Código:** En lugar de escribir e implementar variaciones idénticas de una clase `ListaDeEnteros`, `ListaDeFlotantes` o `ListaDeFoos`, escribes una única plantilla genérica.

---

## 2. Hook Example

```cpp
#include <iostream>

// 1. Declaracion de una Clase Plantilla (Parameterized Type)
template <typename T>
class Nodo {
private:
    T dato;
    Nodo<T>* siguiente;
public:
    Nodo(T d, Nodo<T>* sig = nullptr) : dato(d), siguiente(sig) {}

    T getDato() const { return dato; }
    Nodo<T>* getSiguiente() const { return siguiente; }
};

// 2. Declaracion de una Funcion Plantilla (Parameterized Function)
template <typename T>
void mostrarDato(const Nodo<T>& nodo) {
    std::cout << "El dato guardado es: " << nodo.getDato() << "\n";
}

int main() {
    // El compilador genera la clase concreta Nodo<int>
    Nodo<int> n1(100);
    mostrarDato(n1); // Deducción automática del parámetro de tipo <int>

    // El compilador genera la clase concreta Nodo<double>
    Nodo<double> n2(3.1416);
    mostrarDato(n2);

    return 0;
}
```

---

## 3. Descomposición Under the Hood

### 3.1 ¿Cómo procesa el compilador un template?
Un template no es código compilado ejecutable; es un patrón de generación:
1. Cuando el compilador encuentra la declaración `Nodo<int>`, realiza la **instanciación** de la plantilla.
2. Copia el cuerpo de la plantilla y sustituye cada aparición del parámetro `T` por `int`.
3. Compila el código resultante generando una clase concreta física en el binario.
- **Ubicación en archivos:** Debido a que el compilador debe poder expandir la plantilla en cualquier unidad de traducción que la consuma, las definiciones de las funciones miembro de los templates **deben residir en el archivo de cabecera (`.h`)** y no en el archivo de implementación (`.cpp`).

---

## 4. Teoría: Plantillas de Clases y Funciones

### 4.1 Sintaxis y placeholders
- Se antepone la directiva `template <typename T>` o `template <class T>` (ambas palabras clave son equivalentes en este contexto).
- Se pueden pasar parámetros que no son tipos (non-type template parameters), como constantes enteras: `template <typename T, size_t N> class ArregloEstatico;`.

### 4.2 Reglas excepcionales de sintaxis
Dentro de la clase plantilla, el nombre de la clase sin los caracteres `<T>` es asumido automáticamente como parametrizado por el compilador únicamente en tres lugares:
1. Al declarar el constructor de la clase.
2. Al declarar el destructor de la clase.
3. Inmediatamente después de la palabra clave `class` al inicio de la declaración.
En cualquier otro contexto (como retornos de métodos o parámetros externos), se requiere usar la firma parametrizada completa `NombreClase<T>`.

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Herencia Privada de Contenedores `void*`
Como resalta *Secrets of C++ Masters*, para evitar el *code bloat* (hinchamiento del tamaño del ejecutable por generar cientos de especializaciones idénticas de 1000 líneas), se implementa un contenedor no parametrizado de punteros `void*` en un `.cpp` compilado. Luego, se escribe una plantilla delgada que herede de forma `private` de este contenedor, delegando las llamadas y realizando casts estáticos seguros. Esto reduce el código generado en la plantilla a simples funciones inline de 1 línea.

### 5.2 Nivel Aplicado: Especialización de Plantillas
Permite redefinir la implementación de una plantilla para un tipo de dato específico (por ejemplo, optimizar `Nodo<bool>` para guardar bits individuales en lugar de bytes completos):
```cpp
template <>
class Nodo<bool> {
    // Lógica optimizada exclusiva para booleanos
};
```

### 5.3 Nivel Complejo: Metaprogramación con SFINAE (Substitution Failure Is Not An Error)
C++ permite habilitar o deshabilitar sobrecargas de funciones plantilla basándose en las propiedades de los tipos de datos en tiempo de compilación.
- Si una sustitución de tipo falla durante la resolución de sobrecargas de plantillas, el compilador no genera un error; Únicamente descarta esa opción y sigue buscando entre las demás sobrecargas disponibles.
- Esto permite realizar introspección de tipos (por ejemplo, verificar si una clase posee un método `.dibujar()` antes de intentar llamarlo, decidiendo la lógica en tiempo de compilación).
- *Nota moderna:* Desde C++20, SFINAE ha sido simplificado mediante el uso de **Concepts y Constraints** (`requires`), permitiendo expresar restricciones de tipos con una sintaxis limpia.

---

## 6. Diseño de Sistemas
Los templates son la base de la biblioteca estándar de C++ (**Standard Template Library - STL**) y de las técnicas avanzadas de programación como la optimización de expresiones matemáticas en tiempo de compilación (*Expression Templates*).

---

## Exercises

### Exercise 1 — Implementar una pila genérica con herencia segura
Diseña una clase base no parametrizada `PilaBase` que guarde punteros `void*` y gestione la estructura del stack. Luego, implementa una clase plantilla `Pila<T>` derivada privadamente de `PilaBase` que exponga métodos públicos `push(T* elemento)` y `T* pop()` fuertemente tipados.

---

## 7. Errores Comunes y Anti-Patrones
- **Intentar separar la definición del template en un `.cpp` tradicional:** Producirá errores de enlace (*linker errors*), ya que el compilador no podrá instanciar el código al procesar otros archivos.
- **Code Bloat descontrolado:** Instanciar plantillas pesadas con docenas de tipos diferentes sin delegar la lógica pesada a clases base no parametrizadas.

---

## 8. Conclusión

---

---

*Next: `33 — Screams (I-O en C++).md` — Flujos de entrada/salida estándar.*
