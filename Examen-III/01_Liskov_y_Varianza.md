# Principio de Sustitución de Liskov — Teoría de Tipos y Varianza en C++

El Principio de Sustitución de Liskov (LSP) es la piedra angular del polimorfismo seguro. Establece las reglas matemáticas para que la herencia oriente el comportamiento de forma predecible sin romper los contratos del programa.

---

## 1. Teoría Formal del LSP

La definición estricta dictamina que si `S` es un subtipo de `T`, los objetos de tipo `T` pueden ser sustituidos por objetos de tipo `S` sin alterar las propiedades del programa. Liskov no es solo compatibilidad de firmas, es compatibilidad de comportamiento y preservación de invariantes.

Un diseño robusto exige que la clase derivada no endurezca las precondiciones (lo que exige del cliente) ni debilite las postcondiciones (lo que promete al cliente). Si la clase derivada impone restricciones inesperadas, el código cliente fallará silenciosamente, obligando al uso de anti-patrones como la comprobación dinámica de tipos para evitar errores ([15 — Principios SOLID y Liskov uno por uno.md](/C++/15_-_Principios_SOLID_y_Liskov_uno_por_uno.md) en sección de Precondiciones).

### 1.1 El Contrato de Subtipado

El subtipado impone garantías matemáticas sobre el estado del objeto. Las invariantes de la clase base deben preservarse o fortalecerse en la subclase.

- Precondiciones no pueden ser más fuertes en el subtipo
- Postcondiciones no pueden ser más débiles en el subtipo
- Invariantes del supertipo deben ser preservadas por el subtipo

---

## 2. Co-varianza y Contra-varianza

El polimorfismo dinámico requiere flexibilizar temporalmente la igualdad estricta de tipos para permitir la especialización de métodos. Esta flexibilización se rige por la co-varianza y la contra-varianza.

### 2.1 Co-varianza (Tipos de Retorno)

La co-varianza permite que una función sobreescrita en una clase derivada retorne un tipo más específico que el método original de la clase base. C++ soporta co-varianza exclusivamente a través de punteros o referencias, ya que el retorno por valor cortaría los datos derivados (slicing).

```cpp
class Animal {
public:
    virtual Animal* clonar() const { return new Animal(*this); }
};

class Perro : public Animal {
public:
    Perro* clonar() const override { return new Perro(*this); } // Retorno co-variante legal
};
```

El cliente espera un `Animal*`, y recibir un `Perro*` cumple matemáticamente con ese contrato, garantizando la seguridad en tiempo de ejecución.

### 2.2 Contra-varianza (Tipos de Argumentos)

La contra-varianza es el mecanismo inverso: permite que una función sobreescrita acepte un tipo más general o base del que pedía la función original. C++ no soporta contra-varianza en funciones virtuales; cualquier intento de hacerlo resulta en la creación de una sobrecarga completamente nueva.

```cpp
class Forma {};
class Circulo : public Forma {};

class ProcesadorBase {
public:
    virtual void procesar(Circulo* c) {}
};

class ProcesadorDerivado : public ProcesadorBase {
public:
    virtual void procesar(Forma* f) {} // C++ lo interpreta como sobrecarga, no sobreescritura
};
```

Si intentáramos sobreescribir cambiando el tipo del argumento, el compilador dejaría la función virtual original oculta y declararía un método independiente, rompiendo la cadena polimórfica.

---

## 3. Violación de LSP en C++

C++ es un lenguaje pragmático que permite saltarse las reglas teóricas si el programador lo solicita explícitamente, lo que facilita violaciones sistémicas del LSP si no se diseña con cuidado.

### 3.1 El Peligro de la Herencia Privada

La herencia privada (`private inheritance`) rompe intencionalmente el contrato "es-un" para implementar un diseño "se implementa en términos de" ([14 — Herencia Privada y Composición.md](/C++/14_-_Herencia_Privada_y_Composición.md) sección de Teoría).

```cpp
class ListaBase { /* ... */ };
class ColaEspecial : private ListaBase { /* ... */ };

void operar(ListaBase* ptr);

// En el main:
ColaEspecial cola;
// operar(&cola); // ERROR: Invalida LSP. Conversión implícita a base inalcanzable.
```

La herencia privada esconde la interfaz base del mundo exterior, impidiendo el polimorfismo dinámico. Obligar a que un puntero de subclase se comporte como la clase base en este contexto resultaría en un error de compilación.
