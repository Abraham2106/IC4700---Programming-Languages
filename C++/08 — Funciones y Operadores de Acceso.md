# 08 — Funciones y Operadores de Acceso

> **Resumen Ejecutivo:** El diseño de firmas de funciones y el uso correcto de los operadores de acceso (`.` y `->`) constituyen la base del flujo de datos en C y C++. Entender cuándo se utiliza cada operador nos previene de fallos de compilación recurrentes al trabajar con referencias, punteros e instancias.
>
> **Prerrequisitos:** Haber leído [04 — Punteros en C y C++](<04 — Punteros en C y C++.md>).
> **Clasificación:** TEMA DE DETALLE

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Conceptos Previos](#conceptos-previos)
- [Hook Example](#hook-example)
- [Descomposición Under the Hood](#descomposición-under-the-hood)
- [Teoría: Firmas y Acceso a Miembros](#teoría-firmas-y-acceso-a-miembros)
- [Progresión de Complejidad](#progresión-de-complejidad)
- [Diseño de Sistemas](#diseño-de-sistemas)
- [Proyecto Aplicado](#proyecto-aplicado)
- [Ejercicios](#ejercicios)
- [Errores Comunes y Anti-Patrones](#errores-comunes-y-anti-patrones)
- [Conclusión y Checklist Mental](#conclusión-y-checklist-mental)

---

## Introducción

### ¿Qué es este tema?
- **Paso de Parámetros:** Mecanismo para suministrar datos a una función (por valor, puntero o referencia).
- **Operador Punto (`.`):** Acceso a miembros sobre instancias directas u objetos referenciados.
- **Operador Flecha (`->`):** Acceso a miembros sobre variables de tipo puntero.

### ¿Por qué importa?
Elegir la forma incorrecta de pasar argumentos (por ejemplo, pasar por valor estructuras gigantes) degrada el rendimiento de la CPU debido a copias innecesarias en el Stack. Por otro lado, confundir `.` y `->` es una de las mayores fuentes de frustración de sintaxis para los principiantes.

---

## Conceptos Previos
- Diferencia física entre un objeto y un puntero a ese objeto.

---

## Hook Example

```cpp
#include <iostream>

struct Punto {
    int x;
    int y;
};

int main() {
    Punto p1{10, 20};
    Punto* ptr = &p1;

    // Acceso directo a la instancia usando el punto
    std::cout << p1.x << "\n";

    // Acceso a través de puntero usando la flecha
    std::cout << ptr->x << "\n";

    // Equivalencia manual (desreferenciar y luego usar punto)
    std::cout << (*ptr).x << "\n"; // Los paréntesis son obligatorios debido a la precedencia del punto.
    return 0;
}
```

---

## Descomposición Under the Hood

### ¿Qué traduce el compilador al usar `->`?
- El operador `ptr->x` es azúcar sintáctica para `(*ptr).x`.
- En ensamblador, el compilador:
  1. Toma la dirección base almacenada en el registro del puntero `ptr` (ej. `RDX`).
  2. Le suma el desplazamiento (offset) del miembro `x` dentro de la definición de la estructura (para el primer miembro el offset suele ser 0).
  3. Ejecuta una lectura indirecta de memoria en la dirección resultante (`RDX + offset`).

---

## Teoría: Firmas y Acceso a Miembros

### 1. Mecanismos de paso de parámetros
- **Por Valor (`void f(int x)`):** Crea una copia local del objeto. Seguro pero costoso para objetos grandes.
- **Por Puntero (`void f(int* p)`):** Pasa la dirección de memoria. Permite valores nulos (`nullptr`).
- **Por Referencia (`void f(int& r)`):** Alias seguro. Sintaxis limpia, no permite nulos.

### 2. Los operadores de acceso
- Usamos el **Punto (`.`)** si la expresión a la izquierda es una instancia real de clase/struct o una referencia.
- Usamos la **Flecha (`->`)** si la expresión a la izquierda es un puntero. En C++, clases personalizadas (como Smart Pointers) pueden sobrecargar el `operator->` para imitar el comportamiento de punteros crudos.

---

## Progresión de Complejidad

### Nivel Simple: Paso por valor vs referencia constante
```cpp
#include <string>

// Lento: Copia los caracteres del string
void procesar_valor(std::string s); 

// Rápido: Pasa un puntero implícito constante de solo lectura
void procesar_ref(const std::string& s); 
```

### Nivel Aplicado: Sobrecarga del Operador Flecha en Smart Pointers
Los Smart Pointers sobrecargan `->` para que parezca que estás usando un puntero ordinario mientras manejas el recurso por detrás.
```cpp
#include <memory>
#include <iostream>

struct Robot {
    void encender() { std::cout << "Robot encendido.\n"; }
};

int main() {
    std::unique_ptr<Robot> r = std::make_unique<Robot>();
    r->encender(); // Llama a operator->() de unique_ptr, retornando Robot* y aplicando ->
    return 0;
}
```

### Nivel Complejo: Puntero a Miembro (Pointer-to-Member)
C++ permite almacenar punteros directamente hacia los miembros o funciones de una estructura/clase en vez de a instancias completas.
```cpp
#include <iostream>

struct Datos {
    int valor;
};

int main() {
    int Datos::*ptr_miembro = &Datos::valor; // Puntero al miembro relativo
    
    Datos d{42};
    std::cout << d.*ptr_miembro << "\n"; // Acceso indirecto sobre la instancia d
    return 0;
}
```

---

## Diseño de Sistemas
En el diseño de interfaces de programación de aplicaciones (APIs), pasar configuraciones complejas por referencia constante (`const Config&`) es mandatorio para mantener el consumo de memoria del Stack controlado en hilos de ejecución ligeros.

---

## Ejercicios

### Ejercicio 1 — Corregir Sintaxis de Acceso
Identifica y corrige los operadores de acceso (`.` o `->`) erróneos en el siguiente código para que compile correctamente.

```cpp
#include <iostream>

struct Jugador {
    std::string nombre;
    int puntuacion;
};

void mostrar_puntos(const Jugador* j) {
    // TODO: Corregir el acceso al miembro de puntuacion del puntero j
    // std::cout << j.puntuacion << "\n";
}

int main() {
    Jugador player{"Kael'thas", 9999};
    Jugador* ptr = &player;
    
    // TODO: Corregir el acceso al nombre a través del puntero ptr
    // std::cout << ptr.nombre << "\n";
    
    mostrar_puntos(ptr);
    return 0;
}
```

---

## Errores Comunes y Anti-Patrones
- **No verificar nulos antes de usar `->`:** Intentar llamar a `ptr->metodo()` cuando `ptr` es `nullptr` causa inmediatamente un crash catastrófico de segmentación en runtime.
- **Escribir `(*ptr).miembro` sin paréntesis:** Escribir `*ptr.miembro` se parsea como `*(ptr.miembro)`, lo que causará un error de compilación inmediato porque `ptr` no tiene ningún miembro directo (al ser un puntero).

---

## Conclusión y Checklist Mental
- [ ] ¿Puedes explicar por qué `ptr->x` y `(*ptr).x` son semánticamente idénticos?
- [ ] ¿Sabes cuándo debes usar el operador `.` sobre una referencia en vez del operador `->`?
- [ ] ¿Por qué es preferible pasar estructuras complejas por referencia constante en lugar de por valor?

---

*Siguiente tema sugerido: [09 — Structs en C y C++](<09 — Structs en C y C++.md>)*


