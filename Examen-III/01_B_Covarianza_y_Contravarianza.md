# Principio de Sustitución de Liskov — Covarianza y Contravarianza

La adherencia matemática al Principio de Sustitución de Liskov (LSP) a nivel de sistema de tipos da origen a dos conceptos fundacionales que determinan la seguridad de la herencia en lenguajes polimórficos: la **Covarianza** y la **Contravarianza**.

Estos términos describen cómo fluye la herencia de una clase compuesta (por ejemplo, el retorno de un método o una colección) en relación con la herencia de sus componentes internos.

---

## 1. El Sistema de Subtipado

Asumamos una jerarquía de clases clásica:
- La clase **Base** (`Animal`)
- La clase **Derivada** (`Perro`, que hereda de `Animal`)

Decimos que `Perro` $\le$ `Animal` (Perro es un subtipo de Animal). 

El problema surge cuando analizamos funciones, punteros o arreglos que operan sobre estos tipos. ¿Es un `List<Perro>` un subtipo válido de `List<Animal>`? Las reglas de covarianza y contravarianza resuelven esto apoyándose en las postcondiciones y precondiciones de Liskov.

---

## 2. Covarianza (Retornos y Salidas)

La **Covarianza** significa que la especialización del contenedor/función va en la **misma dirección** que la jerarquía original. Preserva el orden lógico (De Base a Derivada).

La covarianza es matemáticamente segura en **Postcondiciones** (valores de retorno de métodos) y en estructuras de **Solo Lectura** (Read-only).

### 2.1 Covarianza en Retorno de Métodos
Si un método sobreescrito (override) en una clase derivada devuelve un tipo *más específico* que el de la clase base, es completamente seguro.

```cpp
class ClonadorAnimal {
public:
    virtual Animal* clonar() = 0; // Promete devolver al menos un Animal
};

class ClonadorPerro : public ClonadorAnimal {
public:
    // Seguro: Un Perro ES un Animal. Liskov se cumple intacto.
    // La derivada no entrega menos que la base.
    Perro* clonar() override; 
};
```
Esta propiedad en C++ se conoce explícitamente como **Tipos de Retorno Covariantes**. Permite evitar conversiones (*casts*) engorrosas en el código cliente.

### 2.2 Covarianza en Estructuras de Solo Lectura
Si una función toma un arreglo genérico *inmutable*, es seguro pasarle un arreglo de un subtipo:
- ¿Puedo leer de un `const Animal*[]` un arreglo que internamente es `Perro*[]`? Sí. Todos los elementos leídos serán al menos `Animal`.

---

## 3. Contravarianza (Parámetros y Entradas)

La **Contravarianza** significa que la dirección de la jerarquía se **invierte** (De Derivada a Base). 

La contravarianza es matemáticamente segura en **Precondiciones** (parámetros de entrada a una función) y en estructuras de **Solo Escritura** (Write-only).

### 3.1 Contravarianza en Parámetros
Liskov estipula que una clase derivada no puede imponer restricciones más fuertes en su entrada. Por consiguiente, es lógicamente seguro que un método derivado acepte un parámetro **más genérico** que su contraparte en la clase base.

*Nota: C++ estático prohíbe la contravarianza directa de argumentos en el override por reglas de resolución de sobrecarga (Overload Resolution), pero lenguajes dinámicos o delegados en otros ecosistemas sí lo soportan formalmente.*

```text
// Concepto Teórico
Clase Base:       virtual void procesar(Perro* p);
Clase Derivada:   virtual void procesar(Animal* a) override; // Relajando la precondición
```
Si el cliente de la clase Base promete entregar un `Perro`, la clase Derivada, que ahora acepta cualquier `Animal` genérico, manejará sin problema al perro, ya que un perro es un animal. Liskov no es violado porque la subclase exige *menos* (acepta más cosas).

### 3.2 El Desastre de la Covarianza en Entradas
Si invirtiéramos la lógica (exigiendo algo más específico en la entrada de la derivada), romperíamos el contrato de Liskov por endurecimiento de precondiciones:
```text
Clase Base:       virtual void alimentar(Animal* a);
Clase Derivada:   virtual void alimentar(Gato* g) override; // ILEGAL
```
El cliente genérico que usa un puntero base creerá que es seguro llamar a `alimentar(Perro)`, pero la instancia derivada crashearía, porque solo acepta `Gatos`. 

---

## 4. Invarianza: El Problema Físico de los Arreglos Mutables

La **Invarianza** ocurre cuando una estructura exige **exactamente** el mismo tipo sintáctico, sin tolerar ni covarianza ni contravarianza.

Esta es la regla inquebrantable para estructuras de **Lectura y Escritura simultáneas**, como los arreglos mutables. 

Si un arreglo mutable fuera covariante (permitir tratar un `Perro[]` como si fuera un `Animal[]`), un cliente engañado podría cometer un error catastrófico de memoria:

```cpp
void arruinar_granja(Animal** corral) {
    // El cliente tiene un puntero a Animal**, así que asume que
    // lógicamente puede insertar un Gato.
    corral[0] = new Gato(); 
}

int main() {
    Perro* manada[5]; // Arreglo en memoria diseñado exclusivamente para Perros.
    
    // Si C++ permitiera que esto compilara (Covarianza Mutable):
    arruinar_granja(manada); 
    
    // Al intentar ladrar, el elemento 0 (que en realidad ahora es un Gato)
    // causará corrompimiento de memoria (Segmentation Fault).
    manada[0]->ladrar(); 
}
```

Debido a que un arreglo mutable permite inserción (exigiendo contravarianza de entrada) y lectura (exigiendo covarianza de salida), la única intersección matemática lógicamente pura que satisface Liskov en ambos frentes es la **Invarianza Estricta**. Un `Perro[]` **no** es un `Animal[]`.

---

## 5. Aplicación Directa al Examen — Instructor / ProfesorMaestria (Examen P5)

### 5.1 El caso planteado

Dada la jerarquía `UnidadDePosgrado <: Unidad` y `CursoMaestria <: Curso`:

```
class Instructor {
    imparte(Unidad) -> List(Curso)
}
class ProfesorMaestria extends Instructor {
    imparte(UnidadDePosgrado) -> List(CursoMaestria)   // ¿cumple LSP?
}
```

### 5.2 Análisis por componente

Las reglas de varianza para redefinir métodos respetando el LSP son:

- **Parámetros (entrada):** deben ser **iguales o más generales** que los del padre → **contravarianza**.
- **Retorno (salida):** debe ser **igual o más específico** que el del padre → **covarianza**.

| Parte redefinida | Padre → Hijo | Requerido por LSP | ¿Cumple? |
|---|---|---|---|
| **Parámetro** | `Unidad` → `UnidadDePosgrado` (se estrecha) | Contravariante (igual o más general) |  **Viola LSP** |
| **Retorno** | `List(Curso)` → `List(CursoMaestria)` (se especializa) | Covariante (igual o más específico) |  Correcto |

### 5.3 ¿Por qué viola el LSP el parámetro?

Un cliente que tiene una referencia de tipo `Instructor` espera poder llamar a `imparte(cualquierUnidad)`. Si en tiempo de ejecución el objeto real es un `ProfesorMaestria` que solo acepta `UnidadDePosgrado`, la llamada con una `Unidad` base **fallaría** — el subtipo no es sustituible.

### 5.4 Versión que sí cumple el LSP

```
class ProfesorMaestria extends Instructor {
    imparte(Unidad) -> List(CursoMaestria)
    //       ↑                  ↑
    // Parámetro igual al padre  Retorno más específico (covariante )
}
```

El parámetro se mantiene en `Unidad` (acepta todo lo que acepta el padre) y el retorno se especializa a `List(CursoMaestria)` — covarianza permitida.

### 5.5 Nota sobre C++

C++ **soporta covarianza de retorno** para punteros y referencias (tipos de retorno covariantes), pero **no soporta contravarianza de parámetros** en overrides: cambiar el tipo de un parámetro crea una **sobrecarga** (`overload`), no una sobreescritura (`override`), lo que puede ocultar silenciosamente el método base.

