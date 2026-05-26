# Guia de Rust: Sintaxis, Ownership y Borrowing

Esta guia resume los temas base de Rust necesarios para resolver problemas pequenos y entender el modelo de memoria del lenguaje.

Temas cubiertos:

- sintaxis basica
- tipos primitivos
- funciones
- estructuras de control
- `match`
- `struct` y `enum`
- `String` vs `&str`
- ownership
- borrowing
- referencias mutables e inmutables

## 1. Variables e inmutabilidad

En Rust, las variables son inmutables por defecto:

```rust
fn main() {
    let x = 5;
    println!("{}", x);
}
```

Si una variable debe cambiar, se usa `mut`:

```rust
fn main() {
    let mut x = 5;
    x = 10;
    println!("{}", x);
}
```

### Idea clave

- `let` crea una variable inmutable
- `let mut` crea una variable mutable

Esto ayuda a evitar cambios accidentales en el programa.

## 2. Tipos basicos

Algunos tipos comunes en Rust:

- `i32`, `i64`, `u64` para enteros
- `f64` para numeros de punto flotante
- `bool` para booleanos
- `char` para un caracter
- `&str` para cadenas prestadas
- `String` para cadenas duenias de sus datos

Ejemplo:

```rust
fn main() {
    let entero: i32 = 10;
    let real: f64 = 3.14;
    let activo: bool = true;
    let letra: char = 'a';
    let saludo: &str = "hola";
    let nombre: String = String::from("Rust");

    println!("{} {} {} {} {} {}", entero, real, activo, letra, saludo, nombre);
}
```

## 3. `&str` vs `String`

### `&str`

- es una referencia a texto
- normalmente apunta a una cadena literal
- no posee los datos

Ejemplo:

```rust
let s: &str = "hola";
```

### `String`

- posee sus datos
- vive en el heap
- puede crecer y modificarse

Ejemplo:

```rust
let s = String::from("hola");
```

### Regla practica

- use `&str` cuando solo necesita leer texto
- use `String` cuando necesita poseer o modificar el texto

## 4. Funciones

Rust exige tipos explicitos en parametros y retorno:

```rust
fn suma(a: i32, b: i32) -> i32 {
    a + b
}

fn main() {
    let r = suma(2, 3);
    println!("{}", r);
}
```

La ultima expresion sin `;` se retorna implicitamente.

## 5. Control de flujo

### `if`

```rust
fn main() {
    let n = 7;

    if n % 2 == 0 {
        println!("par");
    } else {
        println!("impar");
    }
}
```

### `while`

```rust
fn main() {
    let mut n = 3;

    while n > 0 {
        println!("{}", n);
        n -= 1;
    }
}
```

### `for`

```rust
fn main() {
    for x in 1..=4 {
        println!("{}", x);
    }
}
```

### `loop`

```rust
fn main() {
    let mut n = 0;

    loop {
        n += 1;
        if n == 3 {
            break;
        }
    }
}
```

## 6. `match`

`match` obliga a cubrir todos los casos posibles:

```rust
fn describe(n: i32) -> &'static str {
    match n {
        0 => "cero",
        1 => "uno",
        _ => "otro",
    }
}
```

Esto hace que Rust sea muy seguro en tiempo de compilacion.

## 7. Structs y enums

### `struct`

```rust
struct Persona {
    nombre: String,
    edad: u32,
}

fn main() {
    let p = Persona {
        nombre: String::from("Ana"),
        edad: 20,
    };

    println!("{} {}", p.nombre, p.edad);
}
```

### `enum`

```rust
enum Estado {
    Activo,
    Inactivo,
}

fn main() {
    let e = Estado::Activo;

    match e {
        Estado::Activo => println!("activo"),
        Estado::Inactivo => println!("inactivo"),
    }
}
```

## 8. Problema aplicado: media, maximo y minimo

Programa que recibe un vector de enteros y retorna `(f64, i32, i32)`:

```rust
fn resumen(nums: &[i32]) -> (f64, i32, i32) {
    let mut suma = 0;
    let mut max = nums[0];
    let mut min = nums[0];

    for &n in nums {
        suma += n;
        if n > max {
            max = n;
        }
        if n < min {
            min = n;
        }
    }

    let media = suma as f64 / nums.len() as f64;
    (media, max, min)
}

fn main() {
    let v = vec![3, 8, 1, 6];
    let r = resumen(&v);
    println!("{:?}", r);
}
```

### Observacion

Si intenta modificar el vector sin declararlo mutable, el compilador reporta un error porque Rust no permite cambiar una variable inmutable.

## 9. Ownership

Ownership es la idea central de Rust.

### Regla de oro

Cada valor tiene exactamente un owner. Cuando el owner sale del scope, el valor se libera automaticamente.

Ejemplo simple:

```rust
fn main() {
    let s = String::from("hola");
}
```

Al terminar `main`, `s` sale del scope y Rust libera la memoria.

## 10. Move semantics

Para tipos como `String`, una asignacion mueve la propiedad:

```rust
fn main() {
    let s1 = String::from("hola");
    let s2 = s1;
}
```

Despues de:

```rust
let s2 = s1;
```

`s1` deja de ser valido. Ahora el owner es `s2`.

## 11. Analisis del codigo con error

Considere este codigo:

```rust
fn main() {
    let s1 = String::from("hola");
    let s2 = s1;

    println!("{}", s1, s2);
}
```

### Error 1: uso despues de mover

`String` no implementa `Copy`, porque sus datos viven en el heap. Entonces:

```rust
let s2 = s1;
```

mueve la propiedad de `s1` a `s2`.

Despues de eso:

- `s1` ya no es valido
- usar `s1` produce error de compilacion

### Error 2: formato incorrecto en `println!`

La linea:

```rust
println!("{}", s1, s2);
```

tiene un solo marcador `{}` pero dos argumentos.

La forma correcta seria:

```rust
println!("{} {}", s1, s2);
```

Pero incluso eso seguiria fallando por el problema de ownership.

## 12. Formas correctas de arreglarlo

### Opcion A: clonar

Si quiere conservar ambos valores:

```rust
fn main() {
    let s1 = String::from("hola");
    let s2 = s1.clone();

    println!("{} {}", s1, s2);
}
```

Aqui:

- `s1.clone()` crea una copia profunda
- `s1` sigue siendo valido
- `s2` tiene su propio `String`

### Opcion B: borrowing inmutable

Si no necesita que `s2` sea otro `String`, puede prestar una referencia:

```rust
fn main() {
    let s1 = String::from("hola");
    let s2 = &s1;

    println!("{} {}", s1, s2);
}
```

Aqui:

- `s1` sigue siendo el owner
- `s2` solo presta acceso de lectura

## 13. Tipos `Copy`

Tipos pequenos como `i32`, `bool` y `char` suelen implementar `Copy`.

```rust
fn main() {
    let x = 5;
    let y = x;

    println!("{} {}", x, y);
}
```

Esto funciona porque `x` se copia, no se mueve.

## 14. Borrowing

Borrowing significa prestar acceso a un valor sin transferir ownership.

### Referencia inmutable

```rust
fn print_len(s: &String) {
    println!("{}", s.len());
}

fn main() {
    let s = String::from("hola");
    print_len(&s);
    println!("{}", s);
}
```

El `String` sigue perteneciendo a `main`.

## 15. Borrowing mutable

Si una funcion debe modificar el valor, recibe `&mut T`:

```rust
fn append_world(s: &mut String) {
    s.push_str(" world");
}

fn main() {
    let mut s = String::from("hola");
    append_world(&mut s);
    println!("{}", s);
}
```

Aqui:

- `main` sigue siendo el owner
- la funcion solo recibe un prestamo mutable

## 16. Error conceptual comun con ownership en funciones

Considere esta version:

```rust
fn append_world(s: String) {
    println!("{}", s);
}
```

Si una funcion recibe `s: String`, toma ownership del argumento.

Eso significa que el llamador pierde el valor:

```rust
fn append_world(s: String) {
    println!("{}", s);
}

fn main() {
    let s = String::from("hola");
    append_world(s);
    println!("{}", s);
}
```

La ultima linea falla, porque `s` fue movido a la funcion.

### Si se quiere conservar el valor

Hay varias opciones:

#### Solo lectura

```rust
fn print_text(s: &str) {
    println!("{}", s);
}
```

o:

```rust
fn print_text(s: &String) {
    println!("{}", s);
}
```

#### Modificacion sin perder ownership

```rust
fn append_world(s: &mut String) {
    s.push_str(" world");
}
```

## 17. Reglas del borrow checker

Regla importante:

- puede haber muchas referencias inmutables `&T`
- o una sola referencia mutable `&mut T`
- pero no ambas al mismo tiempo sobre el mismo valor

### Valido

```rust
fn main() {
    let s = String::from("hola");
    let r1 = &s;
    let r2 = &s;

    println!("{} {}", r1, r2);
}
```

### Invalido

```rust
fn main() {
    let mut s = String::from("hola");
    let r1 = &s;
    let r2 = &mut s;

    println!("{} {}", r1, r2);
}
```

Esto falla porque mezcla un prestamo inmutable con uno mutable sobre el mismo valor.

## 18. Caso de uso: leer y modificar un `String`

Queremos:

- una funcion que lea el texto
- otra que lo modifique
- y que `main` pueda seguir usando el `String`

```rust
fn leer(s: &String) {
    println!("Texto actual: {}", s);
}

fn modificar(s: &mut String) {
    s.push_str(" world");
}

fn main() {
    let mut texto = String::from("hola");

    leer(&texto);
    modificar(&mut texto);
    leer(&texto);

    println!("Final: {}", texto);
}
```

### Por que no pueden usarse simultaneamente

No puede existir al mismo tiempo:

- una referencia mutable `&mut texto`
- y otra referencia activa, mutable o inmutable

La razon es evitar data races y estados inconsistentes.

## 19. Factorial iterativo

```rust
fn factorial(n: u64) -> u64 {
    let mut acc = 1;

    for i in 1..=n {
        acc *= i;
    }

    acc
}
```

## 20. Factorial recursivo

```rust
fn factorial(n: u64) -> u64 {
    if n == 0 {
        1
    } else {
        n * factorial(n - 1)
    }
}
```

## 21. Factorial con `match`

```rust
fn factorial(n: u64) -> u64 {
    match n {
        0 => 1,
        _ => n * factorial(n - 1),
    }
}
```

## 22. Ejemplo de funcion `es_primo`

```rust
fn es_primo(n: u64) -> bool {
    if n < 2 {
        return false;
    }

    let mut i = 2;
    while i * i <= n {
        if n % i == 0 {
            return false;
        }
        i += 1;
    }

    true
}
```

## 23. Shadowing

Rust permite redeclarar una variable con el mismo nombre:

```rust
fn main() {
    let x = 5;
    let x = x + 1;
    let x = x * 2;

    println!("{}", x);
}
```

Esto se llama `shadowing`.

### Diferencia con mutabilidad

- con `mut`, cambia la misma variable
- con shadowing, se crea una nueva variable con el mismo nombre

## 24. Resumen final

Ideas que debe dominar:

- Rust es de tipado fuerte y estatico
- las variables son inmutables por defecto
- `String` y `&str` no son lo mismo
- `match` exige cobertura completa
- `String` normalmente se mueve, no se copia
- borrowing permite usar valores sin transferir ownership
- `&T` sirve para leer
- `&mut T` sirve para modificar
- no se pueden mezclar prestamos incompatibles al mismo tiempo

Si entiende estos puntos, ya tiene la base correcta para problemas pequenos y para casi todas las preguntas introductorias de Ownership y Borrowing.
