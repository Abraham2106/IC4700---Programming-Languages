# Polimorfismo y Funciones Virtuales — Vtables, vptrs y el coste de la indirección en el despacho dinámico

El polimorfismo dinámico permite invocar comportamientos derivados a través de punteros a clases base mediante despacho dinámico en tiempo de ejecución. Este mecanismo se implementa mediante tablas virtuales (vtables) que introducen una indirección de memoria.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
El **polimorfismo dinámico** (runtime polymorphism) permite que un mismo fragmento de código opere sobre objetos de diferentes tipos derivados a través de un interfaz de clase base común. La selección de qué función concreta invocar ocurre en **tiempo de ejecución**, no en compilación.

### 1.2 ¿Por qué importa?
- **Extensibilidad:** Puedes añadir nuevas clases derivadas (ej. un nuevo tipo de `Pago`) sin modificar el código cliente que trabaja con la interfaz base (`Pago::procesar()`).
- **Riesgo crítico sin `virtual`:** Olvidar declarar el destructor de la clase base como `virtual` es uno de los errores de diseño C++ más frecuentes y destructivos. Provoca que al destruir un objeto derivado a través de un puntero de la base, solamente se invoque el destructor de la base, dejando los recursos del objeto derivado sin liberar.

---

## 2. Hook Example

```cpp
#include <iostream>

class Animal {
public:
    virtual void hablar() const {          // Virtual: despacho dinámico
        std::cout << "...\n";
    }
    virtual ~Animal() = default;           // Destructor virtual: SIEMPRE necesario en bases polimórficas
};

class Perro : public Animal {
public:
    void hablar() const override {          // override: verificación en compilación
        std::cout << "¡Guau!\n";
    }
};

class Gato : public Animal {
public:
    void hablar() const override {
        std::cout << "¡Miau!\n";
    }
};

void hacer_hablar(const Animal& a) {
    a.hablar();   // Selección dinámica en runtime: Perro ? Guau, Gato ? Miau
}

int main() {
    Perro p;
    Gato g;
    hacer_hablar(p);    // Imprime: ¡Guau!
    hacer_hablar(g);    // Imprime: ¡Miau!
    return 0;
}
```

---

## 3. Descomposición Under the Hood

### 3.1 La Tabla Virtual (vtable) y el Puntero Virtual (vptr)

Cuando el compilador detecta al menos una función `virtual` en una clase, transforma la memoria del objeto de la siguiente manera:

1. **vtable:** Genera una tabla estática (en la sección de datos del ejecutable) por cada clase que contiene funciones virtuales. Esta tabla es un arreglo de punteros a función, cada entrada apuntando a la implementación más derivada disponible para cada función virtual.

2. **vptr:** Inserta silenciosamente un puntero adicional (`vptr`) dentro de cada objeto de esa clase. Este puntero ocupa típicamente 8 bytes (en 64 bits) y apunta a la vtable de la clase concreta del objeto. Esto es el único overhead de memoria del polimorfismo dinámico.

3. **Despacho Virtual:** Al ejecutarse `a.hablar()` a través de un puntero de clase base:
   - La CPU carga el `vptr` del objeto desde memoria.
   - Dereferencia ese puntero para acceder a la vtable de la clase concreta.
   - Lee la entrada correspondiente a `hablar()` de la vtable.
   - Realiza un salto indirecto (`JMP [vtable + offset]`) a la implementación correcta.

```
Objeto Perro en Heap:
+------------+
|   vptr ----+----> vtable de Perro
+------------+       [0] ? Perro::hablar()
|  ...datos  |       [1] ? ~Perro()
+------------+

Objeto Gato en Heap:
+------------+
|   vptr ----+----> vtable de Gato
+------------+       [0] ? Gato::hablar()
|  ...datos  |       [1] ? ~Gato()
+------------+
```

---

## 4. Teoría: Virtual, Override y Clases Abstractas

### 4.1 1. La palabra clave `virtual`
Indica al compilador que la resolución de la función debe diferirse al runtime consultando la vtable del objeto real, no el tipo estático del puntero/referencia.

### 4.2 2. La palabra clave `override` (C++11)
Verificación en tiempo de **compilación** de que la firma de la función derivada coincide exactamente con una función virtual de la base. Si hay un error tipográfico o discrepancia de firma, el compilador falla con un error claro.
```cpp
class Base {
    virtual void procesar(int x) const;
};

class Derivada : public Base {
    // Sin override: el compilador no avisará si cambias accidentalmente la firma
    void procesar(double x) const;  // Crea una nueva función, NO sobrescribe Base::procesar

    // Con override: el compilador detecta inmediatamente que la firma no coincide
    // void procesar(double x) const override;  // ERROR DE COMPILACIÓN
};
```

### 4.3 3. La palabra clave `final` (C++11)
- Sobre una función: impide que clases derivadas la sobrescriban.
- Sobre una clase: impide que se herede de ella.
```cpp
class Concreto final : public Base {};    // Nadie puede heredar de Concreto
void metodo() final;                       // Nadie puede sobreescribir este método
```

### 4.4 4. Funciones Virtuales Puras y Clases Abstractas
Una función virtual pura (`= 0`) convierte la clase en **abstracta**: no se pueden instanciar objetos directamente de ella. Las clases derivadas están obligadas a proporcionar una implementación concreta.
```cpp
class Figura {
public:
    virtual double area() const = 0;    // Función virtual pura
    virtual ~Figura() = default;
};

// double f = Figura{};   // ERROR: Figura es abstracta, no se puede instanciar

class Circulo : public Figura {
    double radio;
public:
    Circulo(double r) : radio(r) {}
    double area() const override {
        return 3.14159 * radio * radio;
    }
};
```

### 4.5 5. El Destructor Virtual: Regla Absoluta
**Siempre** que una clase pueda ser usada de forma polimórfica (es decir, que se manipulen punteros o referencias de la clase base que apunten a objetos derivados), su destructor **debe** ser `virtual`.

```cpp
class BaseNoVirtual {
public:
    ~BaseNoVirtual() { /* Solo se invocará este destructor */ }
};

class Derivada : public BaseNoVirtual {
    int* recursos_derivados;
public:
    Derivada() { recursos_derivados = new int[100]; }
    ~Derivada() { delete[] recursos_derivados; } // NUNCA se llamará ? MEMORY LEAK
};

int main() {
    BaseNoVirtual* ptr = new Derivada();
    delete ptr;    // Solo llama ~BaseNoVirtual(). Los 100 ints del Heap quedan huérfanos.
}
```

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Polimorfismo en Colecciones
El poder real del polimorfismo se revela al manejar colecciones heterogéneas de objetos derivados.
```cpp
#include <vector>
#include <memory>

int main() {
    std::vector<std::unique_ptr<Animal>> zoologico;
    zoologico.push_back(std::make_unique<Perro>());
    zoologico.push_back(std::make_unique<Gato>());
    zoologico.push_back(std::make_unique<Perro>());

    for (const auto& animal : zoologico) {
        animal->hablar();    // Despacho virtual correcto para cada tipo
    }
    // Todos los unique_ptrs se destruyen automáticamente (RAII)
}
```

### 5.2 Nivel Aplicado: `dynamic_cast` y RTTI
C++ permite verificar en runtime el tipo real de un objeto polimórfico usando `dynamic_cast`. Si el cast es inválido, devuelve `nullptr` (para punteros) o lanza `std::bad_cast` (para referencias).
```cpp
#include <iostream>

Animal* a = new Perro();

// dynamic_cast verifica en runtime si 'a' apunta realmente a un Perro
if (Perro* perro = dynamic_cast<Perro*>(a)) {
    perro->ladrar(); // Acceso seguro al comportamiento específico de Perro
} else {
    std::cout << "No es un Perro.\n";
}

delete a;
```

### 5.3 Nivel Complejo: CRTP (Curiously Recurring Template Pattern)
Una alternativa de polimorfismo estático (sin vtable) que resuelve el tipo correcto en tiempo de compilación usando templates, eliminando el overhead del despacho dinámico.
```cpp
// Polimorfismo estático: cero overhead de vtable en runtime
template<typename Derivada>
class AnimaBase {
public:
    void hablar() const {
        static_cast<const Derivada*>(this)->hablar_impl();
    }
};

class PerroCRTP : public AnimaBase<PerroCRTP> {
public:
    void hablar_impl() const { std::cout << "¡Guau CRTP!\n"; }
};
```

---

## 6. Diseño de Sistemas

### 6.1 Comparativa: Polimorfismo Dinámico vs Estático

| Característica | Virtual (Dinámico) | CRTP / Overloading (Estático) |
|---|---|---|
| **Resolución** | Runtime (vtable lookup) | Compile-time |
| **Overhead** | ~1 indirección por llamada + vptr | Cero overhead |
| **Colecciones heterogéneas** | Sí (`vector<Base*>`) | No (requiere tipos homogéneos) |
| **Extensibilidad sin recompilar** | Sí (DLL plugin systems) | No |
| **Uso típico** | APIs de framework, plugins | Matemáticas, contenedores internos |

---

## 7. Proyecto Aplicado

### 7.1 Sistema de Renderizado Polimórfico
```cpp
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

class Forma {
public:
    virtual double area() const = 0;
    virtual void dibujar() const = 0;
    virtual ~Forma() = default;
};

class Circulo : public Forma {
    double radio;
public:
    explicit Circulo(double r) : radio(r) {}
    double area() const override { return 3.14159 * radio * radio; }
    void dibujar() const override {
        std::cout << "Dibujando circulo de area: " << area() << "\n";
    }
};

class Rectangulo : public Forma {
    double ancho, alto;
public:
    Rectangulo(double w, double h) : ancho(w), alto(h) {}
    double area() const override { return ancho * alto; }
    void dibujar() const override {
        std::cout << "Dibujando rectangulo de area: " << area() << "\n";
    }
};

void renderizar_escena(const std::vector<std::unique_ptr<Forma>>& formas) {
    double area_total = 0;
    for (const auto& f : formas) {
        f->dibujar();
        area_total += f->area();
    }
    std::cout << "Area total de la escena: " << area_total << "\n";
}

int main() {
    std::vector<std::unique_ptr<Forma>> escena;
    escena.push_back(std::make_unique<Circulo>(5.0));
    escena.push_back(std::make_unique<Rectangulo>(4.0, 6.0));
    escena.push_back(std::make_unique<Circulo>(2.5));

    renderizar_escena(escena);
    return 0;
}
```

---

## Exercises

### Exercise 1 — Implementar Jerarquía con Destructor Virtual
El siguiente código tiene un memory leak crítico. Identifica la causa exacta y corrígela.

```cpp
#include <iostream>

class Vehiculo {
public:
    // BUG: Falta declarar este destructor como virtual
    ~Vehiculo() {
        std::cout << "Destructor Vehiculo\n";
    }
};

class Camion : public Vehiculo {
    int* carga_datos;
public:
    Camion() {
        carga_datos = new int[1000];
        std::cout << "Camion creado con 1000 ints\n";
    }
    ~Camion() {
        delete[] carga_datos;
        std::cout << "Destructor Camion: datos liberados\n";
    }
};

int main() {
    // Observa: puntero de base que apunta a un objeto derivado
    Vehiculo* v = new Camion();
    delete v;    // ¿Qué destructores se invocan? ¿Por qué hay un leak?
    return 0;
}
```

### Exercise 2 — Diseñar una Clase Abstracta
Diseña una interfaz abstracta `Serializable` con un método virtual puro `serializar()` y dos clases concretas `SerializadorJSON` y `SerializadorXML` que la implementen. Luego escribe una función `guardar(const Serializable& s, const std::string& destino)` que funcione de forma polimórfica.

```cpp
// TODO: Declarar la clase abstracta Serializable
// TODO: Implementar SerializadorJSON y SerializadorXML
// TODO: Implementar la función guardar()
```

---

## 8. Errores Comunes y Anti-Patrones

### 8.1 Error #1: Destructor de base no virtual (el más común y destructivo)
```cpp
// Malo: Destructor no virtual ? Memory Leak garantizado
class Base {
    ~Base() {}    // Si alguien hace: Base* b = new Derivada(); delete b; ? LEAK
};

// Bueno: Siempre virtual en clases polimórficas
class Base {
    virtual ~Base() = default;
};
```

### 8.2 Error #2: Llamar funciones virtuales en constructores/destructores
```cpp
class Base {
public:
    Base() {
        hablar();    // Llama Base::hablar(), NO Derivada::hablar()
                     // porque el objeto Derivada aún no existe
    }
    virtual void hablar() { std::cout << "Base\n"; }
};

class Derivada : public Base {
    void hablar() override { std::cout << "Derivada\n"; }
};

// Derivada d; ? Imprime "Base", no "Derivada". Comportamiento sorpresivo.
```

### 8.3 Error #3: Confundir override con overloading
```cpp
class Base {
    virtual void procesar(int x);
};

class Mala : public Base {
    // Sin 'override': el compilador cree que es una nueva función, no una sobrescritura.
    // Si la firma cambia por error, el polimorfismo silenciosamente deja de funcionar.
    void procesar(int x);

    // Con 'override': el compilador verifica la coincidencia de firma en compilación.
    void procesar(int x) override;    // Correcto
};
```

---

## 9. Conclusión

**Los tres puntos más críticos:**
1. El polimorfismo dinámico tiene un costo real de hardware: un puntero extra (vptr) por objeto y una indirección extra (vtable lookup) por llamada virtual. Úsalo cuando lo necesites, no de forma refleja.
2. El destructor `virtual` en clases base polimórficas no es opcional: es una regla invariable. Olvidarlo es un bug de memory leak garantizado.
3. Siempre usa `override` al sobrescribir funciones virtuales: es la única defensa estática contra errores silenciosos de firma.

---

*Has completado el módulo final de esta serie. Felicitaciones.*

*Para profundizar más: Revisar [01 — Diferencias entre C, C++ y Rust](<01 — Diferencias entre C, C++ y Rust.md>) con los nuevos conocimientos adquiridos para ver cómo estos mecanismos de bajo nivel diferencian filosóficamente a C++ de Rust y C.*

---

*Next: `23 — Destructores (Invocacion directa de destructores Explicita e Implicita, definicion de operadores).md` — Invocación directa de destructores y su papel en placement new.*
