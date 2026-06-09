# Orden de Construcción y Destrucción (Depth) — Secuencia determinista de constructores y destructores

El orden en que se inicializan los miembros de una clase y sus bases sigue reglas estrictas basadas en su declaración, no en la lista de inicialización del constructor. Comprender esta secuencia determina la validez de los punteros y recursos compartidos durante el nacimiento del objeto.

---

## 1. Introducción

### 1.1 ¿Qué es este tema?
Define el orden exacto en el que el runtime de C++ invoca los constructores de las clases bases y los miembros internos al instanciar un objeto, y el orden inverso para sus destructores al liberarlo.

### 1.2 ¿Por qué importa?
Si inicializas un miembro miembro usando el valor de otro miembro que se declara más abajo en la definición de la clase, leerás memoria sin inicializar (datos basura), desatando comportamiento indefinido (UB) en tiempo de compilación.

---

## 2. Hook Example

```cpp
#include <iostream>

struct Base {
    Base() { std::cout << "1. Constructor Base\n"; }
    ~Base() { std::cout << "6. Destructor Base\n"; }
};

struct Miembro {
    Miembro() { std::cout << "2. Constructor Miembro\n"; }
    ~Miembro() { std::cout << "5. Destructor Miembro\n"; }
};

struct Derivada : public Base {
    Miembro m;
    Derivada() { std::cout << "3. Constructor Derivada\n"; }
    ~Derivada() { std::cout << "4. Destructor Derivada\n"; }
};

int main() {
    {
        Derivada d;
    } // d sale de scope aquí
    return 0;
}
```

---

## 3. Descomposición Under the Hood

### 3.1 ¿Qué hace el compilador tras bambalinas?
- El compilador reordena silenciosamente el código dentro de tu constructor para ajustarse a las reglas del estándar:
  1. Inserta llamadas implícitas a los constructores de las clases bases (en orden de declaración de herencia).
  2. Inserta llamadas a los constructores de las variables miembro (en orden exacto de declaración dentro del cuerpo del struct/class).
  3. Ejecuta el cuerpo de código escrito por el programador dentro de las llaves del constructor `{}` de la clase derivada.
- Para el destructor, el compilador genera exactamente la secuencia inversa de llamadas.

---

## 4. Teoría: Reglas de la Secuencia de Vida

### 4.1 1. El Orden de Construcción
Al crear un objeto de clase derivada:
1. **Clases bases virtuales** (si existen en herencia múltiple compleja).
2. **Clases bases no virtuales** (en orden de aparición de izquierda a derecha en la firma de herencia).
3. **Variables miembro de la clase** (en el orden estricto de declaración dentro del cuerpo de la clase, **independientemente** de cómo las listes en la lista de inicialización de miembros del constructor).
4. **Cuerpo del constructor** (código dentro del bloque `{}`).

### 4.2 2. El Orden de Destrucción
Ocurre en sentido **estrictamente inverso** al de construcción:
1. **Cuerpo del destructor** de la clase derivada.
2. **Variables miembro** en orden inverso a su declaración.
3. **Clases bases no virtuales** en orden inverso a su declaración.
4. **Clases bases virtuales** en orden inverso.

---

## 5. Progresión de Complejidad

### 5.1 Nivel Simple: Inicialización errónea por orden de lista
```cpp
#include <iostream>

class Contador {
    int minimo;
    int actual;
public:
    // ERROR SUTIL: 'actual' se declara después de 'minimo' en la clase, pero aquí se intenta
    // inicializar 'minimo' usando 'actual'. 'actual' aún contiene basura al evaluar minimo(actual).
    Contador(int val) : actual(val), minimo(actual) {
        std::cout << "Minimo: " << minimo << "\n"; // Imprimirá basura
    }
};
```

### 5.2 Nivel Aplicado: Jerarquía de Herencia Múltiple
```cpp
class Dispositivo {};
class Impresora : public Dispositivo {};
class Escaner : public Dispositivo {};
// Construcción: Dispositivo -> Impresora -> Dispositivo -> Escaner
```

---

## 6. Diseño de Sistemas
En motores de físicas u otras aplicaciones críticas, se debe evitar llamar a métodos virtuales desde el constructor o destructor de una clase base. Dado que el objeto derivado aún no se ha construido (o ya se destruyó), la llamada resolverá estáticamente al método de la clase base, no al de la derivada, lo que suele considerarse un error de diseño de ciclo de vida.

---

## Exercises

### Exercise 1 — Predecir e Imprimir la Secuencia
Ordena mentalmente las salidas del siguiente fragmento de código. Agrégale los mensajes a consola en constructores/destructores y comprueba si tu predicción física coincide.

```cpp
#include <iostream>

struct ComponenteA {
    ComponenteA() { std::cout << "ComponenteA\n"; }
    ~ComponenteA() { std::cout << "~ComponenteA\n"; }
};

struct ComponenteB {
    ComponenteB() { std::cout << "ComponenteB\n"; }
    ~ComponenteB() { std::cout << "~ComponenteB\n"; }
};

struct SistemaBase {
    SistemaBase() { std::cout << "SistemaBase\n"; }
    ~SistemaBase() { std::cout << "~SistemaBase\n"; }
};

struct SuperSistema : public SistemaBase {
    ComponenteB cb;
    ComponenteA ca;
    
    SuperSistema() : ca(), cb() {
        std::cout << "SuperSistema\n";
    }
    ~SuperSistema() {
        std::cout << "~SuperSistema\n";
    }
};

int main() {
    {
        SuperSistema ss;
    }
    return 0;
}
```

---

## 7. Errores Comunes y Anti-Patrones
- **Asumir que el orden de inicialización sigue la lista del constructor:** El compilador ignora tu lista de inicialización `: miembro2(x), miembro1(y)` y siempre inicializará `miembro1` primero si está declarado antes en el archivo `.h`. Los compiladores modernos emiten warnings sobre esto (`-Wreorder`).

---

## 8. Conclusión

---

---

*Next: `17 — Constructores en C++.md` — Inicializadores y constructores delegados.*
