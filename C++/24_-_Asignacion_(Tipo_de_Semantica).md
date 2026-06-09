# Asignación y Semántica de Copia — Inicialización frente a asignación y la semántica de valor profundo

La asignación sobreescribe el estado de un objeto preexistente, a diferencia de la inicialización que lo construye desde cero. Distinguir la semántica de copia superficial de la profunda previene la compartición no intencionada de recursos en el heap y errores de corrupción de memoria.

---

## 1. Inicialización vs Asignación

Es común confundir la inicialización con la asignación debido al uso sintáctico del signo de igualdad. El compilador de C++ las diferencia de forma estricta según el ciclo de vida del objeto.

La inicialización ocurre cuando el objeto está naciendo en memoria por primera vez. Esto invoca constructores (incluyendo constructores por copia), garantizando que el objeto se construya con estado válido antes de cualquier uso.

La asignación ocurre sobre un objeto completamente construido que ya posee recursos en ejecución. Esto invoca el operador de asignación (`operator=`), el cual debe encargarse de limpiar o liberar los recursos viejos antes de tomar propiedad de los nuevos.

### 1.1 Semántica de Valor por Defecto

C++ emplea semántica de valor por defecto. Esto significa que asignar un objeto a otro implica copiar su estado lógico completo, en claro contraste con lenguajes manejados (como Java) donde la asignación simplemente copia una referencia (puntero superficial).

---

## 2. Copia Superficial vs Profunda

El peligro fundamental de la semántica de copia radica en cómo se manejan los recursos dinámicos. Por defecto, el compilador genera una copia miembro a miembro (Shallow Copy).

Si un objeto contiene un puntero a memoria en el heap, una copia superficial duplicará la dirección de memoria, haciendo que ambos objetos apunten al mismo recurso físico. Cuando el primer objeto se destruya, invocará `delete` sobre ese recurso, dejando al segundo objeto con un puntero colgante y provocando doble liberación (Double Free) cuando este intente destruirse.

### 2.1 Implementación de Copia Profunda (Deep Copy)

Para solucionar esto, las clases que manejan memoria cruda deben proveer un constructor de copia y un operador de asignación explícitos. 

```cpp
class StringSeguro {
    char* datos;
public:
    StringSeguro(const StringSeguro& otro) {
        datos = new char[std::strlen(otro.datos) + 1];
        std::strcpy(datos, otro.datos);
    }
};
```

La copia profunda asigna un nuevo bloque de memoria independiente en el heap y luego transfiere el contenido bit a bit, garantizando que cada objeto administre un ciclo de vida exclusivo.

### 2.2 Diagnóstico de Memoria Fuga

La depuración de punteros compartidos exige visualizar las direcciones reales en ejecución. 

```cpp
void dump_memoria(const char* ptr) {
    std::cout << "Stack: " << &ptr << " -> Heap: " << (void*)ptr << "\n";
}
```

Al imprimir la dirección del puntero (stack) junto al destino del puntero (heap), se evidencia inmediatamente si dos instancias independientes están apuntando a la misma locación física.
