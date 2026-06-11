# Idiosincracias del C++

> Documento analizado y transcrito íntegramente preservando el contenido original.

## Página 1

José CastroIdiosincracias del C++Lenguajes de Programación  


## Página 2

El uso del ConstVariables y Constantes•Variables Constantes   
🤔 (en otros lenguajes se usa el keyword val):  const int j = 17; // Una constante entera j = 29;       // Ilegal, no se puede cambiar, es constante const int i; // Ilegal, debe ser inicializado •const int j =17; es lo mismo que const int j(17);  const i = 17; int& j = i; // Ilegal, ya que j se puede modificar  


## Página 3

El uso del ConstVariables y Constantes•Las dos siguientes no son equivalentes  const int i = 17; // Una constante entera #define i 17;    •La primera es una variable que toma espacio en la memoria, la segunda es una macro, accesar a la constante en memoria toma ciclos extra del reloj, no así con la macro.  
•Constantes Enum: enums no se usan mucho en el lenguaje C por una razón simple: los símbolos deﬁnidos tienen ámbito global y saturan el espacio de nombres rápidamente. En C++ eso no es un problema, ya que los podemos limitar a un ámbito:  class Foo { public:    enum Status { kOpen=0, kClosed }; }; // en el código Foo::Status s = Foo::kOpen; // :: es el operador de resolución de ámbito  


## Página 4

El uso del ConstVariables y Constantes•Constantes Enum: enums no se usan mucho en el lenguaje C por una razón simple: los símbolos deﬁnidos tienen ámbito global y saturan el espacio de nombres rápidamente. En C++ eso no es un problema, ya que los podemos limitar a un ámbito:  class Foo { public:    enum Status { kOpen=0, kClosed }; }; // en el código Foo::Status s = Foo::kOpen; // :: es el operador de resolución de ámbito •Note que nombre a símbolos deben ser referidos explícitamente con su ámbito, esto permite reutilizar kOpen y kClosed. Valores enum son tratados como macros por el compilador, a diferencia de las variables constantes.  


## Página 5

Puntero a constanteVariables y Constantes•Punteros no son tan simples, ya que contienen dos valores, en el siguiente ejemplo tenemos un puntero variable a un valor constante  const int* p; int i = 17; p = &i; // válido *p = 29; // Ilegal •Esto se extiende a estructuras y objetos también  class foo { public:       int x; }; const foo* f = new foo; f->x = 17; // Ilegal, no se puede asignar a miembros  


## Página 6

Puntero constanteVariables y Constantes•Punteros constante es el opuesto, es un puntero que apunta a una posición de memoria que cuyo valor se puede modiﬁcar, pero el puntero mismo no puede cambiar a otra posición  int i = 17; int j = 29; int* const p; // Ilegal, debe ser inicializado int* const p1 = &i; // Ok *p1 = 29; // Se vale, se puede cambiar el valor en esa posición p1 = &j; // Ilegal •El mas restrictivo es el puntero constate a un valor constante, no se puede hacer mucho con esto  int i=17; int j=29; const int* const p; // Ilegal, debe ser inicializado const int* const p1 = &i; // Vale *p1 = 29; // Ilegal! p1 = &j; // Ilegal!  


## Página 7

Parámetros constantes a funcionesVariables y Constantes•Un argumento constante a una función tiene las mismas reglas que otras variables constantes  void f(const int* p) {    *p = 17; // Ilegal    int i = 29;    p = &i; // Vale, pero ¿Para qué va a querer hacer esto? } // En algún lugar del código int i = 17; f(&i); // Ok, el argumento real no requiere ser constante •Si utilizamos referencias en vez de punteros, la semántica es similar  void f(const int& p) {    p = 17; // Ilegal    int i = 29;    p = i; // Vale, pero de nuevo ¿Para qué va a querer hacer esto? } int i = 17; f(i); // Ok  


## Página 8

Parámetros no constantes a funcionesVariables y Constantes•Si el argumento formal a una función se declara que NO es constante, los parámetros actuales no pueden ser constantes.  void int f(int*); int i = 17; const int* p = &i; const int j = 29; f(&i); // Vale, i no es constante f(p); // Ilegal f(&j); // También ilegal, j es constante  Esta es una manera en que el compilador se asegura que: una vez constante, para siempre constante  


## Página 9

Métodos constantesVariables y Constantes•Dentro de un método declarado constante, la variable this se trata como un puntero a constante y el compilador se quejará si se trata de utilizar de una forma no-constante.  class foo { private:    int x; public:    void f() const;    void g(); }; void h(int*); void m(foo*); void foo::f() {     x = 17;    // Ilegal: cambia un miembro     this->g(); // Ilegal: g no es una función constante     h(&x);     // Ilegal: h puede que cambie x     m(this);   // Ilegal: argumento es no-constante en m }  


## Página 10

Objetos en la PilaObjetos en la Pila versus Objetos en el Heap•En C++, a diferencia de C, todos las variables declaradas, aún las de tipos proveídos por el compilador, son instancias de clases, y siguen, con muy pocas excepciones, las mismas reglas que los objetos creados por clases diseñadas e implementadas por el programador.  
•Al crear variables en la pila estas tienen un ámbito de vida bien deﬁnido  {    int i;    foo f(argumentos_del_constructor);    // a punto de salir del bloque, destructor de f y de i se llaman }; •Objetos creados en el stack dejan de existir cuando termina el bloque en que fueron declarados  {    int i;    foo f;    algunaFuncion(&f); //  }; •Este código es peligroso, no hay manera de saber si hay problemas sin antes ﬁjarse en el código de algunaFuncion.  


## Página 11

Objetos en el HeapObjetos en la Pila versus Objetos en el Heap•Para crear objetos en el Heap se utiliza el operador (no es una función, se implementa con una función, pero es un operador) new  foo* f = new foo(argumentos_del_constructor);  
•Esto es bastante simple, el constructor se llama en el momento en que se invoca new, pero ¿Cuándo se invoca el destructor? resulta ser que el C++ ofrece muchas variantes a este problema (y también es uno de los problemas y pulgas más comunes en código C++: destructores no invocados, o invocados dos veces, etc..)  
•Punteros y Referencias son dos maneras distintas de acceder objetos, pero no guardan relación con el objeto en sí.  {    foo f;    foo* p = &f;    f.funcionMiembro(); // utiliza el objeto    p->funcionMiembro(); // utiliza la dirección    p = new foo;    foo& r = *p // una referencia al objeto    r.funcionMiembro(); // lo mismo que p->funcionMiembro();  }; •Estos operadores se llaman operadores de selección de miembros  


## Página 12

Problemas con Objetos en la PilaObjetos en la Pila versus Objetos en el Heap•Si por alguna razón se invoca un destructor sobre un objeto que se encuentra en la pila esto puede tener consecuencias graves, ya que el compilador invoca automáticamente el destructor cuando el objeto sale de ámbito  
•En el mejor de los casos el programa da error y termina.  
•En el peor de los casos el programa sigue funcionando pero empieza a actuar  como si estuviera enojado con uno, borrando cosas aleatorias en la memoria.   
•El problema es que cuando se llama al operador new, este usualmente guarda información del tamaño del bloque que asigna al inicio de la estructura que construye, pero para objetos creados en la pila, esta información no existe (no fueron creados utilizando new), así que el C++ al llamar a delete toma lo que sea que está al inicio de la estructura y lo interpreta como memoria que debe liberar.  


## Página 13

Problemas con Objetos en la PilaObjetos en la Pila versus Objetos en el Heap•Hemos identiﬁcado varios problemas con objetos basados en la Pila en C++  
•Si se toma la dirección de un objeto que se encuentra en la pila, puede que la dirección siga existiendo aún después de que el objeto haya dejado de existir (lenguajes más modernos, como Rust, impiden esto)  
•Si se toma la dirección de un objeto que se encuentra en la pila, esta dirección puede que se le de al operador delete.  
•Una buena regla de dedos es nunca utilice la dirección de un objeto que esta en la pila, o de alguno de sus miembros.  


## Página 14

Ventajas con Objetos en la PilaObjetos en la Pila versus Objetos en el Heap•Por otro lado los objetos en la pila se les asigna espacio y eliminan mucho más rápido que los creados en el Heap.  
•El operador new, por lo menos su versión default, toma ciclos del reloj decidiendo en que espacio asigna el objeto.  
•Sin embargo el operador new y delete se pueden redeﬁnir, e implementar de forma bastante eﬁciente, así que para el programador de C++ conocedor esto no es un inconveniente tan fuerte.  
•El hecho que los objetos en la pila se eliminan automáticamente es otra gran ventaja es tan conveniente que es común hacer abstracciones como la siguiente que generan un objeto en la pila que encapsula uno en el heap.  


## Página 15

Ventajas con Objetos en la PilaObjetos en la Pila versus Objetos en el Heap•El hecho que los objetos en la pila se eliminan automáticamente es otra gran ventaja es tan conveniente que es común hacer abstracciones como la siguiente que generan un objeto en la pila que encapsula uno en el heap.  class PFoo { private:     Foo* f; public:     PFoo* : f(new Foo) {}     ~PFoo() { delete f; }     operator Foo*() { return f; } }; void g(Foo*);  {    PFoo p;    g(p); // invoca el operador Foo*() para convertir    // p es destruido y a su vez el valor Foo* f. } •Este wrapper hace que la construcción del valor en el heap, y su correspondiente destrucción, sea automática.  
•Otra ventaja de objetos en la pila tiene que ver con manejo de excepciones, si hay error, el trace de la pila llama a los destructores en orden, no así si los objetos están en el heap.  


## Página 16

Ambitos y FuncionesIdiosincracias del C++•Una de las ventajas del C++ sobre el C es que permite mucho más control sobre el ámbito de un símbolo, aunque de una manera a veces un poco extraña, de hecho, es un arma de dos ﬁlos, las reglas de ámbito en C++ se pueden volver muy complejas.  
•Un ámbito lo determina cualquiera de los siguientes  
•Una clase  
•Una estructura  
•Una union  
•Un bloque de código  
•El espacio de nombres global  
•Símbolos dentro de un ámbito solo aplican dentro de ese ámbito, esto aplica para cualquier deﬁnición, enums, structs, clases, funciones, …  


## Página 17

ClasesIdiosincracias del C++•Una clase en C++ es más que una estructura de datos, es una idea similar a los módulos (misma idiosincracia tiene el Java, pero no el Python, donde las clases son transparentes)  class Foo { public:    static int y; // una variable global    static void GFn(); // una función global, pero visible solo dentro de Foo    int x; // variable miembro    Foo(); // un constructor    void Fn(); // función miembro    typedef int (*IntFn)(); // un tipo    enum Status { kOpen=0, kClosed }; // otro tipo    struct Bar { // estructura anidada       int a; int b; static void BarFn();    }; private:    void Hn(); };  


## Página 18

ClasesIdiosincracias del C++•class Foo { public:    static int y; // una variable global    static void GFn(); // una función global, pero visible solo dentro de Foo    int x; // variable miembro    Foo(); // un constructor    void Fn(); // función miembro    typedef int (*IntFn)(); // un tipo    enum Status { kOpen=0, kClosed }; // otro tipo    struct Bar { // estructura anidada       int a; int b; static void BarFn();    }; private:    void Hn(); };  Foo::Foo() { GFn(); /* ya estamos en el ámbito de Foo */ }  void f() { Foo::GFn(); /* se requiere el Foo:: */ } •La sintaxis de el operador de resolución de ámbito se puede tornar intimidante  Foo::Bar b; Foo::Bar::BarFn(); •Por este motivo las estructuras anidadas suelen ser o triviales o no-públicas  


## Página 19

ClasesIdiosincracias del C++•class Foo { public:    static int y; // una variable global    static void GFn(); // una función global, pero visible solo dentro de Foo    int x; // variable miembro    Foo(); // un constructor    void Fn(); // función miembro    typedef int (*IntFn)(); // un tipo    enum Status { kOpen=0, kClosed }; // otro tipo    struct Bar { // estructura anidada       int a; int b; static void BarFn();    }; private:    void Hn(); }; •x, Foo(), Fn() son todos miembros de la clase y tienen sentido solo en el contexto de un objeto utilizando el operador . o el ->. GFn() por el contrario, es global, y se puede accesar utilizando directamente el nombre de la clase, pero también, aunque es confuso, como miembro de un objeto.  Foo f; f.GFn(); // es lo mismo que Foo::GFn();   


## Página 20

EstructurasIdiosincracias del C++•Estructuras en en teoría en C++ son lo mismo que clases, incluso se puede derivar de ellas, excepto que todos sus miembros son públicos por defecto.  
•En la práctica ningún programador de C++ que se respete usará estructuras si no es solo para objetos simples donde:  
•No hay miembros virtuales  
•La estructura no deriva de nada, excepto tal vez otra estructura  
•Nada deriva de la estructura, excepto tal vez otra estructura  
•La idea es que estructuras tengan pequeños acumulados de datos que son compatibles hacia atrás con el C puro.  


## Página 21

Uniones Idiosincracias del C++•Unions en C++ son casi iguales que en C  
•Miembros de una union no pueden tener constructores, pero el union en general sí  
•Una union no puede derivar de nadie  
•Nadie puede derivar de una union  
•Destructores de miembros no se llaman, pero el destructor de la unión puede sí.  
•Uniones y métodos derivados no mezclan bien, no tiene sentido crear métodos protegidos o virtuales en uniones  


## Página 22

Ámbito Global Idiosincracias del C++•El espacio global o ámbito global de variables en el C++ es muy complejo, algunos ejemplos  // en Foo.cpp typedef int Simbolo; // en Bar.cpp typedef void (*Simbolo)(); •No hay conﬂicto aquí, ambas deﬁniciones de Simbolo son locales al archivo en que han sido declaradas  


## Página 23

Ámbito Global Idiosincracias del C++•El siguiente, sin embargo, no es válido, ya que Simbolo es una variable y debe ser única para todos los archivos que combina el linker  // en Foo.cpp int Simbolo; // en Bar.cpp void (*Simbolo)(); •Hay una excepción que veremos luego cuando se utiliza overloading, una forma de evitar problemas como el caso anterior es utilizar miembros estáticos  // en Foo.cpp static int Simbolo; // en Bar.cpp static void (*Simbolo)();  


## Página 24

Ámbito Global Idiosincracias del C++•Aunque no es lo común, cualquier nombre declarado en el ámbito global puede ser accesado utilizando el operador de resolución de ámbito sin especiﬁcar el ámbito (asume el global)  ::Fn(); // Llama la función global Fn() int x = ::i; // Asigna el valor de la variable global i ::AlgunTipo y; // Utiliza el tipo declarado globalmente •Resolución explícita siempre precede a cualquier símbolo deﬁnido localmente, ya sea en el bloque o clase  


## Página 25

OverloadingIdiosincracias del C++•El C++ permite redeﬁnir los nombres de funciones de muchas maneras. Clases es una manera, funciones con el mismo nombre en distintas clases no se reﬁere al mismo objeto.  
•Overloading de funciones permite reutilizar el nombre de una función muchas veces dentro del mismo ámbito.  
•Argumentos: dos funciones con el mismo nombre no se consideran la misma si diﬁeren en el número, o tipo de sus argumentos.    


## Página 26

OverloadingIdiosincracias del C++•Argumentos: dos funciones con el mismo nombre no se consideran la misma si diﬁeren en el número, o tipo de sus argumentos.  void Fn(); void Fn(int); void Fn(long); // Se permite si int y long son distintos int Fn(int); // ilegal, solo difiere en valor de retorno int Fn(char*); // Ok, distintos argumentos void Fn(int, char*); void Fn(char*, int); // Ok, argumentos en distinto orden void Fn(char* s, int x, int y=17); // Ok, 3 argumentos Fn(“hola”, 17); // Error - coincide con 2 signaturas  


## Página 27

Funciones ConstantesOverloading•Una función constante del mismo nombre que una que no lo es, se considera una función distinta.  class Foo { public:    void Fn();    void Fn() const; }; Foo* f = new Foo; f->Fn(); // Llama a la versión no constante const Foo* f1 = f; f1->Fn(); // Llama a la versión constante  


## Página 28

Visibilidad: herencia privadaIdiosincracias del C++•El C++ tiene las visibilidades de public: private: y protected: que ya hemos visto, sin embargo, hay casos especiales, que veremos a continuación.  
•Derivación privada: cuando se deriva de forma privada de una clase base todos sus miembros protegidos se vuelven privados y no son re-publicados a sub-clases.  
•También es ilegal hacer un cast de una clase derivada a uno de sus clases bases privadas.  class Mixin { private:     int x; protected:     int y; public:     Mixin();     void A*(); }; class Foo : private Mixin {…}; class Bar : public Foo {…};  


## Página 29

Visibilidad: herencia privadaIdiosincracias del C++•class Mixin { private:     int x; protected:     int y; public:     Mixin();     void A*(); }; class Foo : private Mixin {…}; class Bar : public Foo {…}; •x solo es visible dentro de Mixin, y es visible dentro de Foo, ningún miembro de Mixin es visible en las clases derivadas de Foo.  
•Este modelo de visibilidad quiebra (no cumple) el principio de sustitución de Liskov de los principios SOLID de OO.  


## Página 30

Visibilidad: re-declaración de miembros virtualesIdiosincracias del C++•No sucede a menudo, pero se puede redeclarar miembros virtuales con una visibilidad distinta que la de la clase base  
•class Foo { protected:     virtual void Fn(); } class Bar: public Foo { public:     virtual void Fn(); }; •Fn() es protected en Foo pero public en Bar, esto no es posible con miembros que no son virtuales  


## Página 31

Visibilidad: re-declaración de miembros no-virtualesIdiosincracias del C++•Cuando se re-declara un dato, o un método que no es virtual, este esconde la versión de la clase base  class Foo { private:     int x; public:     void Fn(); } class Bar: public Foo { private:     int x;  // un segundo miembro llamado x public:     void Fn(); // una segunda función llamada Fn() }; // en el código del cliente Bar *b = new Bar; b->Fn();    // llama Bar::Fn(); Foo* f = b; // válido, Foo es una clase base pública f->Fn();    // llama a Foo::Fn();  


## Página 32

Visibilidad: re-declaración de miembros virtualesIdiosincracias del C++•Si un método no es virtual en la clase base, es mala idea declararlo virtual en clases heredas (porqué lo permite? ni idea). Si un método se declara virtual en la case base, seguirá siendo virtual en las subclases aunque no se declaren virtual ahí (basta con declararlo virtual en la case base) DWIMNWIS: do what I mean, not what I say   
😀  class Foo { public:     virtual void Fn(); } class Bar: public Foo { public:     void Fn(); // tratado como virtual aunque no lo diga }; // en el código del cliente Bar *b = new Bar; b->Fn();    // llama Bar::Fn(); Foo* f = b; // válido, Foo es una clase base pública f->Fn();    // también llama a Bar::Fn();  


## Página 33

Visibilidad: re-declaración de miembrosIdiosincracias del C++•Si se crea método no con distinta signatura pero con el  mismo nombre que otro en la clase base, entonces esconde a todas las versiones de la clase base. El siguiente código es permitido por el C++, pero no es nada recomendable.  class Foo { public:     virtual void Fn();     virtual void Fn(char*); } class Bar: public Foo { public:     virtual void Fn(int); // porqué lo permite? no sé }; •Se se trata de llamar a Fn() desde Bar*, no se puede, se puede hacer un cast de Bar a Foo, en cuyo caso ahora se pueden acceder a las dos funciones en Foo, pero no la de Bar  


## Página 34

ConstructoresTipos y Operadores•Constructores se pueden ver de dos ángulos: como una función que inicializa el objeto, o del punto de vista matemático como un mapeo de los argumentos del constructor al dominio de la clase  
•La segunda manera ayuda a entender mejor ciertas características de ellos, tal como operadores de conversión  
•Las reglas de los constructores son bastante complejas, pero todo programador de C++ las tiene que entender a fondo para evitar pulgas y sesiones largas de debugging  


## Página 35

Constructores sin argumentosTipos y Operadores•Si la clase provee un constructor sin argumentos, el constructor se usa por defecto en tres circunstancias: class Foo { public:     Foo(); }; •class Bar : public Foo { // 1. clase base public:    Bar(); }; •class BarBar { private:    Foo f; // 2. miembro de clase }; •Foo f; // 3. Instancia creada de Foo Foo* f1 = new Foo; // 3. Mismo caso  


## Página 36

Constructores con argumentosTipos y Operadores•Constructores son como cualquier función en el sentido que pueden ser sobrecargados. Se pueden declarar todos los constructores que quiera  
•La única diferencia real es que es que la signatura de un constructor no devuelve valor y no pueden ser declarados const  
•Si se declara un constructor que acepta argumentos pero no se declara el constructor sin argumentos, el compilador no permitirá crear objetos de la clase, aún como base de otra clase, sin utilizar argumentos  


## Página 37

Constructores con argumentosTipos y Operadores•class Foo {  public:    Foo(char*); }; Foo f; // Error - no hay constructor sin argumentos class Bar : public Foo {  public:    Bar(); }; Bar::Bar() { // Error - no hay constructor sin argumentos para Foo }  


## Página 38

Listas de Iniciación de MiembrosTipos y Operadores•Para resolver estos problemas, el C++ utiliza listas de inicialización de miembros class Foo { public:    Foo(char*); }; class Bar : public Foo { public:    Bar(char*) }; class BarBar { private:    Foo f;    int x; public:    BarBar() }; •Bar::Bar(char *) : Foo(s) {…} •BarBar::BarBar() : f(“Hola”), x(17) {…}  


## Página 39

Listas de Iniciación de MiembrosTipos y Operadores•¿Porqué no escribimos: BarBar::BarBar() : f(“Hola”) {    x = 17;    … } en vez de BarBar::BarBar() : f(“Hola”), x(17) {…}?  
•Resulta que la segunda versión es levemente más eﬁciente  
•La primera versión inicializa x con el constructor sin argumentos (inicializa a 0) y luego le asigna 17  
•Los miembros inicializados en la lista de inicialización no son inicializados con el constructor sin argumentos  


## Página 40

Orden de construcciónTipos y Operadores•Si una clase no provee ningún constructor, se inicializa como si el compilador hubiera proveído el constructor sin argumentos  
•El constructor sin argumentos invoca el constructor sin argumentos de la clase base y de sus miembros  
•El orden de construcción está bien deﬁnido ya sea que utiliza constructores default, sobrecargados, sin argumentos, o con argumentos  
•constructores de clases base se invocan en el orden en que aparecen en la lista de derivación  
•construtores de miembros son invocados en el orden en que son declarados dentro de la clase  
•una vez que se invocan los constructores de las clase base y de los miembros, el código de su constructor se ejecuta  
•Esta lógica se aplica recursivamente, profundidad primero,  a todas las clases base  


## Página 41

Constructores por copiaTipos y Operadores•Constructores por copia es un constructor especial deﬁnido por su signatura class Foo { public:     Foo(const Foo&); }; Foo::Foo(const Foo& f) …  
•El constructor por copia se utiliza para hacer copia de objetos, esto puede ocurrir en varias circunstancias void Fn(Foo f) {…} void Gn(Foo& f) {…} Foo f; Foo f1(f); Foo f2 = f; // Construcción, esto no es asignación! Fn(f); // Invoca el constructor por copia para pasar un Foo por valor const Foo f3; Gn(f3); // constructor por copia para enviar una versión no const de f3  


## Página 42

Constructores por copiaTipos y Operadores•El constructor por copia proveído por el compilador tiene una secuencia bien deﬁnida para llamar a constructores de clases base y miembros de la clase  
•constructores por copia de las clases base son invocados en el orden que se encuentran las clases en la lista de derivación  
•constructores por copia de miembros son invocados en el orden que están dentro de la clase  
•Esta lógica se aplica recursivamente, profundidad primero  
•La verdad: es igual que cualquier otro constructor  


## Página 43

Constructores por copiaTipos y Operadores•A diferencia de otro tipo de constructores, el compilador es orgulloso y celoso de sus constructores por copia  
•Si se sobrecarga el constructor por copia, el compilador le exige al programador llamar explícitamente cualquier constructor por copia de las clases base y los miembros  
•A menos que explícitamente se usen constructores por copia en la lista de inicialización, el compilador utilizará el constructor sin argumentos para inicializar las clases base  
•class Foo {…} class Bar : public Foo { private:    Foo f; public:    Bar(const Bar&); }; •// Probablemente mal Bar::Bar(const Bar& b) {     // oops! no hay lista de inicialización, se invocan constructores sin argumentos } •// Probablemente bien Bar::Bar(const Bar& b) : Foo(b), f(b.f) {…}  


## Página 44

Constructores por copiaTipos y Operadores•Hay (pocas) situaciones en la que queremos que se utilice el constructor por defecto, tal como la clase siguiente que serializa los objetos derivados  
•class Serialized { private:    static int ProxSerialNumber;    int serialNumber; public:    Serialized(const Serialized&);    Serialized();    int SerialNumber(); }; •// En Serialized.cpp int Serialized::ProxSerialNumber = 0; Serialized::Serialized()                 : serialNumber(ProxSerialNumber++) {} Serialized::Serialized(const Serialized&): serialNumber(ProxSerialNumber++) {} int Serialized::SerialNumber() { return serialNumber; } •No nos importa cual constructor escogen las clases derivadas, porque hacemos que ambos hagan lo mismo  


## Página 45

Constructores private o protectedTipos y Operadores•Es común declarar constructores como protected para evitar que los generen instancias de una clase  
•Si el constructor es private, solo miembros de la clase o friends pueden crear objetos de la clase  
•Si el constructor es protected, se pueden crear instancias de clases derivadas, ya que los constructores de estas clases pueden llamar al constructor de la clase base  
•El problema con este esquema es que tiene huecos, si el constructor es protected, miembros de la clase y de sus derivados pueden crear objetos de la clase  
•La única manera de lograr que una clase no se pueda instancias (y entonces que sea clase virtual pura), es que la clase tenga por lo menos un miembro virtual puro  


## Página 46

Constructores private o protectedTipos y Operadores•La única manera de lograr que una clase no se pueda instancias (y entonces que sea clase virtual pura), es que la clase tenga por lo menos un miembro virtual puro  
•class Foo { protected:    Foo(); }; class Bar : public Foo { public:    Foo* Fn(); }; Foo* Bar::Fn() {     return new Foo; // funciona a pesar de todos nuestros intentos }  


## Página 47

Instancias AnónimasTipos y Operadores•Una instancia anónima es un objeto que …, bueno, mejor un ejemplo  
•struct Punto {    int X;    int Y;    Punto(int x, int y) : X(x), Y(y) {} };  double distancia(Punto p) {    return sqrt(double(p.X)*double(p.X)               + double(p.Y)*double(p.Y)           ); double d = distancia(Punto(17,29));  


## Página 48

Inicializando Objetos GlobalesIdiosincracias del C++•Las reglas para inicializar objetos globales en C++ son complejas aún leyendo la especiﬁcación del lenguaje  
•Las reglas con las que se puede contar son las siguientes  
•Todas las variables son inicializadas a 0 antes de que se haga cualquier procesamiento  
•Objetos dentro de un vector o estructura son inicializados en el orden que aparecen dentro del vector o estructura  
•Cada objeto global es construido antes de que es utilizado por cualquier código del programa. Queda a criterio del compilador si se inicializan antes del main(), o se inicializan cuando van a ser utilizados por primera vez  
•Objetos globales en la misma “unidad de traducción” (generalmente un archivo .cpp) se inicializan en el orden que aparecen dentro de dicha unidad. El efecto de la regla anterior es que la inicialización puede que se haga unidad por unidad, cuando se utiliza cada unidad por primera vez  
•El mejor consejo es no depender del orden en que se inicializan los objetos globales, y mantener los objetos globales a un mínimo  


## Página 49

Inicializando Objetos GlobalesIdiosincracias del C++•Un truco para evitar problemas en orden de inicialización es el siguiente código  
•// En Library.h class Library { private:    static int count;    static void OpenLibrary();    static void CloseLibrary(); public:    Library();    ~Library(); }; static Library LibraryDummy; inline Library::Library() { if (count++ == 0) OpenLibrary(); } inline Library::~Library() { if (--count == 0) CloseLibrary(); }   


## Página 50

Inicializando Objetos GlobalesIdiosincracias del C++•Un truco para evitar problemas en orden de inicialización es el siguiente código  
•// En Library.cpp int Library::count = 0; // se efectúa antes de cualquier código int unGlobal; Foo* unFooGlobal; void Library::OpenLibrary() {    unGlobal = 17;    unFooGlobal = new Foo; } void Librar::CloseLibrary() {    unGlobal = 0;    delete unFooGlobal;    unFooGlobal = NULL; }   


## Página 51

Inicializando Objetos GlobalesIdiosincracias del C++•El truco anterior toma algo para acostumbrarse, pero es común  
•Una biblioteca que lo usa es iostreams  
•Resuelve el problema de inicializar una vez todas las estructuras de datos para IO antes de que cualquier otro proceso empiece y que esto suceda una sola vez  


## Página 52

DestructoresIdiosincracias del C++•Los destructores se invocan cuando un objeto en el Stack sale de ámbito, o cuando a un objeto en el Heap se le invoca el operador delete, aquí unos cuantos hechos no muy conocidos de destructores  
•Orden de Llamado. los destructores están garantizados a ejecutar en el orden inverso en que fueron llamados los constructores  
•Destrucción de Globales. los destructores de las variables globales se invocan en el orden inverso en que fueron invocados los constructores, cualquiera que haya sido este orden  
•Destructores No Virtuales. C++ utiliza el tipo de la variable que apunta al objeto para determinar que destructor utilizar, si el tipo del punto es base*, utilizara el destructor de la clase base a menos que el destructor sea virtual  


## Página 53

DestructoresIdiosincracias del C++•Destructores No Virtuales. C++ utiliza el tipo de la variable que apunta al objeto para determinar que destructor utilizar, si el tipo del punto es base*, utilizara el destructor de la clase base a menos que el destructor sea virtual  class Foo { public:    ~Foo() {} }; class Bar : public Foo { private:     int* numeros; public:     Bar() : numeros(new int[17]) {…}    ~Bar() { delete [] numeros; } }; Bar* b = new Bar; delete b; // Llama a Bar::~Bar(); Foo* f = new Bar; delete f; // Llama a Foo:~Foo(); !!  


## Página 54

Invocar Directamente a los DestructoresIdiosincracias del C++•Esto no tiene mucho sentido a menos que empecemos a hacer nuestro propio manejo de memoria, pero es posible llamar a los destructores directamente  
•class Foo { public:   ~Foo(); }; Foo* f = new Foo; f->Foo::~Foo();  


## Página 55

AsignaciónIdiosincracias del C++•Asignar un objeto a otro es cosa seria en C++, el lenguaje tiene muchas reglas a veces no obvias. Aquí algunas  
•Foo f; Foo f1; f1 = f; •En la tercera linea es donde sucede la asignación, si f1 y f2 son objetos básicos (como enteros), C++ nada más copia los contenidos.  
•No sucede así cuando Foo es una clase  


## Página 56

AsignaciónIdiosincracias del C++•No sucede así cuando Foo es una clase   
•class String { private:    char* s; public:    String(char*);   ~String();    void Dump(ostream& os); }; String::String(char* str) : s(NULL) {    if (str == NULL) {        s = new char[1];        s[1] = ‘\0’;    } else {        s = new char[strlen(str)+1];        strcpy(s, str);    } }•Código del cliente  
•String* s1 = new String(“Hola”); String* s2 = new String(“Ciao”); s2 = s1; delete s1; // ups s2->Dump(cout); // :( delete s2; // Nooooo!            


## Página 57

Asignación por defectoIdiosincracias del C++•Así como los constructores, el operador de asignación por defecto tiene un comportamiento bien deﬁnido, la secuencia es la siguiente  
•Se asignan clases base en el orden en que aparecen, utilizará el operator= que deﬁna la clase y si no existe usa el operator= por defecto  
•Se asignan miembros de datos en el orden en que aparecen, utiliza el operator= deﬁnido por las clases y si no existe utiliza el operator= por defecto  
•Esto es a diferencia de otros lenguajes (ie. Haskell) que no tienen copia o comparación por igualdad a menos que la clase explícitamente lo declare  


## Página 58

Sobrecargando la asignación con operator=Idiosincracias del C++•Por el momento la signatura del operador = que nos interesa es X& X::operator=(const X&)  
•class String { private:    char* s; public:    String(char*);   ~String();    String(const String&); // Arreglemos esto de una vez    String& operator=(const String&);    void Dump(ostream& os); }  


## Página 59

Sobrecargando la asignación con operator=Idiosincracias del C++•Por el momento la signatura del operador = que nos interesa es X& X::operator=(const X&)  
•String::String(char* str) : s(NULL) { // Constructor    if (str == NULL) { // la hilera nula       s = new char[1];        s[0] = ‘\0’;    } else {       s = new char[strlen(str)+1];       strcpy(s, str);    } }  


## Página 60

Sobrecargando la asignación con operator=Idiosincracias del C++•Por el momento la signatura del operador = que nos interesa es X& X::operator=(const X&)  
•String::~String() { // Destructor    delete[] s; }  String::String(const String& s1) : s(NULL) {    s = new char[strlen(s1)+1;    strcpy(s, s1.s); }  


## Página 61

Sobrecargando la asignación con operator=Idiosincracias del C++•Por el momento la signatura del operador = que nos interesa es X& X::operator=(const X&)  
•String& String::operator=(const String& s1) {    if (this == &s1) return *this;    delete s; // eliminar valor anterior    s = new char[strlen(s1.s)+1];    strcpy(s, s1.s);    return *this } void String::Dump(ostream& os) {   os <<“\””<<s<<“\””; }  


## Página 62

Sobrecargando la asignación con operator=Idiosincracias del C++•En el ejemplo anterior tanto el constructor por copia como el operador = hacen copias y no comparten memoria con los otros objetos  
•En general un operador de = debe  
•Asegurarse que no se está haciendo una asignación como de x = x. Si son el mismo objeto no debe hacer nada  
•Eliminar con delete cualquier almacenamiento previo  
•Copiar los valores  
•return *this;  


## Página 63

Asignando MiembrosIdiosincracias del C++•Con los miembros de una clase, siempre es bueno asignarlos explícitamente, así el compilador decide si utiliza el operador = por defecto o alguno que haya sido deﬁnido por el programador  class Foo {   public:      Foo& operator=(const Foo&); }; class Bar {   public: // no hay sobrecarga de = }; class FooBar {   private:      Foo f;      Bar b;   public: FooBar& operator=(const FooBar& fb); }  


## Página 64

Asignando MiembrosIdiosincracias del C++•Con los miembros de una clase, siempre es bueno asignarlos explícitamente, así el compilador decide si utiliza el operador = por defecto o alguno que haya sido deﬁnido por el programador   FooBar& FooBar::operator=(const FooBar& fb)  {    if (this == &fb) // si son el mismo objeto no debe hacer nada        return *this;     f = fb.f; // utiliza Foo::operator= sobrecargado    b = fb.b; // utiliza operator= por defecto    return *this; }     


## Página 65

Asignando Clases BaseIdiosincracias del C++•La sintaxis para asignar clases base puede ser un poco engorrosa. Si uno no la ha visto antes, toma rato encontrar la forma apropiada, aquí va:  
•class Foo {…}; class Bar : public Foo { public:    Bar& operator=(const Bar&); }; •Bar& Bar::operator=(const Bar& b) {    if (this == &b) return *this;    this->Foo::operator=(b); // What!!?    return *this; }  


## Página 66

Otras signaturas para operator=Idiosincracias del C++•El operador = se puede sobrecargar con otras signaturas  
•Al hacerlo sirve para mapear un tipo de datos a otro  
•El operador = que toma como parámetro un objeto del mismo tipo sí tiene la propiedad especial que el compilador proporciona uno por defecto, para cualquier otra conversión es necesario escribirla explícitamente  
•class String { // igual que antes public:    String& operator=(const String&); // lo típico    String& operator=(char*); // sobrecargado    String& operator=(int);   // llama a ato };  


## Página 67

La forma funcional de un operadorIdiosincracias del C++•Hay dos maneras de ver un operador como +, ya sea como una forma sintáctica especial o como una función  
•En C++, la forma funcional siempre se escribe con operator seguido del símbolo del operador  
•class Foo {…}; Foo x, y, z; z = x + y; // forma infija (normal) z = operator+(x,y); // función no-miembro z = x.operator+(y); // procedimiento miembro •Las últimas tres son equivalentes, pero el operador debe programarse como un procedimiento miembro o como una función, no puede ser de ambas maneras  


## Página 68

Sobrecargas por funciones no-miembroIdiosincracias del C++•Sobrecargar con funciones que no son miembro requiere deﬁnir una función global, que probablemente ocupa ser  •class String { friend String& operator+(const String&, const String&); private:    char* s; public:    // constructores etc… }; String operator+(const String& s1, const String& s2) {    char* s = new char[strlen(s1.s)+strlen(s2.s)+1];    strcat(s, s1.s, s2.s);    String newStr(s)    delete s;    return newStr; } String s1 = “Hola”; String s2 = “Ciao”; String s3 = s1 + s2;  


## Página 69

Sobrecargas por funciones no-miembroIdiosincracias del C++•Sobrecargar con funciones que no son miembro realmente es igual que cualquier otra función excepto por el nombre extraño  
•Por si tenía la curiosidad, esta es la razón por la cual se invento el friend •Si no fuera friend, no tendría acceso a los elementos privados de la clase  
•De hecho así es como la biblioteca String implementa el operator+  
•Cualquier operador puede ser sobrecargado en C++ como un operador no miembro, excepto por los operadores unarios de conversión =, [], (), y ->, los cuales deben ser sobrecargados como miembros de la clase  


## Página 70

Sobrecargas por funciones miembroIdiosincracias del C++•Sobrecargar con funciones que son miembro requiere se declaran dentro de la clase •class String { private:    char* s; public:    // constructores etc…    String& operator+(const String&) const; }; String String::operator+(const String& s1) {    char* s2 = new char[strlen(s)+strlen(s1.s)+1];    strcat(s2, s, s1.s);    String newStr(s2);    delete s2;    return newStr; } String s1 = “Hola”; String s2 = “Ciao”; String s3 = s1 + s2;  


## Página 71

Sobrecargas por funciones no-miembroIdiosincracias del C++•Sobrecargar con funciones que no miembro realmente es igual que cualquier otro miembro excepto por el nombre extraño  
•Cualquier operador puede ser sobrecargado en C++ como un operador miembro  
•¿Cuál de las dos maneras utilizamos? las razones más comunes para utilizar un operador NO miembro son  
•El primer argumento es un tipo básico, como int o double  
•El primer argumento es un tipo deﬁnido en una biblioteca que no podemos, o no queremos, modiﬁcar  


## Página 72

Sobrecargas por funciones no-miembroIdiosincracias del C++•El compilador busca la sobrecarga de operadores miembro revisando el tipo del parámetro de la izquierda, si su tipo está a la derecha, no le queda más que hacerlo no-miembro  
•El operador no-miembro más común es el << para imprimir, se debe redeﬁnir cada vez que quiero imprimir un tipo   
•ostream& operator<<(ostream& os, const String& Str) {     os << Str.s; // Asume que esta función es un friend     return os; }  


## Página 73

Una notaIdiosincracias del C++•El Operador anterior puede que no funcione si su compilador no declara el strcpy y strcat como constantes, esto obliga a hacer la siguiente declaración  
•String String::operator+(const String& s1) {    char* s2 = new char[strlen((char*)s)                       +strlen((char*)s1.s)+1];    strcat(s2, (char*)s, (char*)s1.s);    String newStr(s2)    delete s2;    return newStr; } •¿Ven lo que sucede cuando no se usa const correctamente?  


## Página 74

Operadores de ConversiónIdiosincracias del C++•Un operador de conversión es especial, se utiliza para mapear un tipo a otro  
•class String { private:    char* s; public:    operator long(); // Utiliza atol para convertir a long }; String operator long() {    // probablemente sea conveniente revisar que la    // hilera sea un número valido, en el rango de long    return atol(s); } String s(“1729”); long x = s; // utiliza el operador long  


## Página 75

Operadores de ConversiónIdiosincracias del C++•Operadores de conversión deben ser miembros  
•Usualmente el compilador averigua solo cual operador utilizar  
•Siempre es preferido utilizar constructores  
•Un operador de conversión debe saber mucho sobre la clase que construye  
•Se utilizan usualmente para tipos básicos  
•El tipo del operador queda deﬁnido por el nombre del operador (que es el tipo mismo)  


## Página 76

Operadores de ConversiónIdiosincracias del C++•Un operador de conversión es especial, se utiliza para mapear un tipo a otro  
•class String { private:    char* s; public:    operator const char*() const { return s; }    operator char*(); }; String::operator char*() {    char* newStr = new char[strlen(s)+1];    strcpy(newStr, s);    return newStr; }   


## Página 77

Resolviendo AmbigüedadIdiosincracias del C++•Si durante la compilación del código el compilador encuentra deﬁniciones de operadores, las resuelve de la siguiente manera  
•Si ambos argumentos son de tipos básicos, utiliza los operadores predeﬁnidos  
•Se el argumento del lado izquierdo es un tipo deﬁnido por el usuario, el compilador busca por un por un miembro que sobrecargue la signatura de la expresión, y si lo encuentra, lo usa.  
•Si esto no funciona, busca un operador no-miembro  
•Si existe ambigüedad, el compilador no le dirá hasta que trate de utilizar el operador  
•Operadores pueden ser declarados virtuales como cualquier otro método, en cuyo caso el compilador ejecuta el método de la clase apropiada dependiendo de la jerarquía de clases  


## Página 78

Operador ->Idiosincracias del C++•El operador -> es especial, veamos primero la sintaxis básica  
•class Puntero  {  private:    Foo* f;  public:    Puntero(Foo* foo) : f(foo) {}    Foo* operator->() const { return f; } }; Puntero p(new Foo); p->miembroDeFoo(); •Esto funciona, el operador devuelve el puntero, y se usa para acceder al miembro  


## Página 79

Operador ->Idiosincracias del C++•La indirección del operador -> se aplica hasta que se encuentre un puntero básico  
•class Puntero2  {  private:    Puntero p;  public:    Puntero2(Foo* foo) : p(foo) {}    Puntero operator->() const { return p; } }; Puntero2 p(new Foo); p->miembroDeFoo(); •Esto funciona también, el operador -> se evalúa tres veces  


## Página 80

Operador []Idiosincracias del C++•El operador [] se puede redeﬁnir para tomar cualquier argumento unitario y devolver cualquier tipo como su valor  
•class String {  private:    char* s;  public:    String(char*);    char operator[](int n) const; // el n-esimo caracter }; char String::operator[](int n) {    // revisar rango    return s[n] }  


## Página 81

Operador []Idiosincracias del C++•Como el operador [] puede tomar solo un argumento, es común utilizar instancias anónimas para simular arreglos multidimensionales  
•struct i3 {  int X, Y, Z;  i3(int x, int y, int z) : X(x), Y(y), Z(z) {} }; class Array3D { // array de 3 dimensiones  private:    // estructura de datos aquí  public:    String& operator[](const i3&); }; String s = anArray[i3(17, 29, 31)];  


## Página 82

Operador ()Idiosincracias del C++•¿A que no sabía que el operador () se podía sobrecargar  
•El lado izquierdo de este operador tiene que ser un objeto que uno quiere tratar como si fuera una función  
•class Funcion {   public:      int operator()(char*); }; •int Funcion::operator()(char* s) {    cout << s;    return 3; } •Funcion fn; int x = fn(“Hooola!”); // imprime Hooola!   


## Página 83

Operador newIdiosincracias del C++•El operador new se puede redeﬁnir  
•Su implementation por defecto tiene la signatura   void* operator new(size_t bytes); •El operador new nunca debe devolver NULL, cuando se le agota la memoria debe generar una excepción  
•Puede ser sobrecargado como función miembro o como friend.  
•Es heredado, así que subclases manejan la memoria de la misma manera que la clase base que sobrecarga el operador  


## Página 84

Operador newIdiosincracias del C++•Es difícil crear código portátil si se sobrecarga el operador new global  
•Así que es preferible sobrecargarlo como miembro  
•class Foo {  public:    void* operator new(size_t bytes); }; void Foo:operator new(size_t bytes) {    if (bytes < MAXBYTES)       // asignación de memoria aquí    else return ::operator new (bytes); } •Esto tiene la ventaja que podemos utilizar el new de biblioteca aún cuando lo redeﬁnimos  


## Página 85

Operador newIdiosincracias del C++•El operador new también puede ser sobrecargado para que acepte un argumento  
•Aunque estas nuevas signaturas no son llamadas automáticamente por el compilador, pueden ser muy útiles para controlar que versión del operador se está utilizando  
•class Pool {  public:    virtual void* Allocate(size_t bytes); }; void* operator new(size_t bytes, Pool* p) {    return p->Allocate(bytes)l } extern Pool* DefaultPool; Foo* = new(DefaultPool) Foo;  


## Página 86

Operador deleteIdiosincracias del C++•El operador delete generalmente se redeﬁne siempre que lo hacemos con el new  
•Hay dos formas de redeﬁnirlo, cualquiera de las dos las tomará automáticamente el compilador cada vez que un objeto es eliminado  
•void operator delete(void* dirección); •void operator delete(void* dirección, size_t bytes); •El primero solo prove el puntero, y se supone que el programador debe saber el tamaño, el segundo da el puntero y el tamaño, pero puede estar equivocado  


## Página 87

Operador deleteIdiosincracias del C++•El primero solo prove el puntero, y se supone que el programador debe saber el tamaño, el segundo da el puntero y el tamaño, pero puede estar equivocado  
•class Foo {   private:      int x;   public:     ~Foo(); }; •class Bar : Public Foo {   private:       int y;   public:      ~Bar() };•Bar* b = new Bar; •delete b; // Tamaño correcto •Foo* f = new Bar; •delete f; // Error!!!     


## Página 88

Reconsiderando PunterosSmart Pointers•class Foo { public:       void miembroDeFoo(); }; Foo * unFoo = new Foo(); unFoo->miembroDeFoo(); •“operator->” proveído por el compilador  
•Que el objeto unFoo es un puntero es un caso especial, podría haber sido cualquier clase que haya deﬁnido el operador ->  


## Página 89

Punteros TontosSmart Pointers•class Integer { private:       int value; public:       Integer() : value(0) {}       Integer(int v) : value(v) {}       operator int() { return value; }       Integer operator+(Integer i)  { return Integer(value+i.value); }       Integer operator+=(Integer i) { value += i.value; return *this; }       // etc }; int f(int); f(Integer(17)); •Bien! cualquier lugar donde pueda utilizar int puedo usar Integer,   


## Página 90

Punteros TontosSmart Pointers•class PFoo { private:       Foo* foo; public:       PFoo() : foo(NULL) {}       PFoo(Foo* f) : foo(f) {}       operator Foo*() { return foo; }       PFoo operator+(ptr_diff offset)   { return PFoo(foo+offset); }       PFoo operator+=(ptr_diff offset)  { foo += offset; return *this; }       // etc }; •Bien! cualquier lugar donde pueda utilizar int puedo usar Integer, o puedo?  PFoo pf(new Foo*); pf->miembroDeFoo(); // no funciona ((Foo*)pf)->miembroDeFoo(); // Funciona pero Ugh!!  


## Página 91

Operador ->Smart Pointers•class PFoo { private:       Foo* foo; public:       PFoo() : foo(NULL) {}       PFoo(Foo* f) : foo(f) {}       operator Foo*() { return foo; }       Foo* operator->()   { return foo; } }; •Bien! cualquier lugar donde pueda utilizar int puedo usar Integer, o puedo?  void f(Foo*) PFoo pf(new Foo*); f(pf); pf->miembroDeFoo();  


## Página 92

Punteros Inteligentes ParametrizadosSmart Pointers•template <Class type>  class Ptr { private:       Type* pointee; public:       Type() : foo(NULL) {}       Ptr(Type* p) : pointee(p) {}       operator Type*() { return pointee; }       Type* operator->()   { return pointee; } }; •Bien! cualquier lugar donde pueda utilizar int puedo usar Integer, o puedo?  void f(Foo*) Ptr<Foo> pf(new Foo*); f(pf); pf->miembroDeFoo();  


## Página 93

Punteros Inteligentes ParametrizadosSmart Pointers•class Bar : public Foo { … };  Ptr<Bar> pf1(new Bar); Ptr<Foo> pf2 = pf1; // funciona, pero no es obvio porqué •Un buen compilador de C++ resuelve esto con la siguiente lógica  
•¿Existe un constructor de Ptr<Foo> que acepta Ptr<bar>? No -> sigamos buscando.  
•¿Hay un operador P<Foo>() en P<Bar>? No -> sigamos buscando.  
•¿Hay una conversión deﬁnida por el usuario desde P<Bar> que calza con la signatura de P<Foo>? Sí! el operator Bar*() convierte un P<Bar> en un Bar*, que a su vez puede ser promovido a un Foo*, que a su vez se puede utilizar en un constructor de P<Foo>;  


## Página 94

Accesando NilAplicaciones•template <Class type>  class Ptr { private:       Type* pointee; public:       Ptr() : pointee(nullptr) {}       Ptr(Type* p) : pointee(p) {}       operator Type*() { return pointee; }       Type* operator->()          {           if (pointee == nullptr) {              cerr << “no se puede accesar miembros de NULL!” << endl;              pointee = new Type;           }            return pointee;        } };  


## Página 95

Generando ExcepcionesAplicaciones•template <Class type>  class Ptr { private:       Type* pointee; public:       enum ErrorType { accesandoNIL };           Ptr() : pointee(nullptr) {}       Ptr(Type* p) : pointee(p) {}       operator Type*() { return pointee; }       Type* operator->()          {           if (pointee == nullptr) throw accesandoNIL;           return pointee;        } };  


## Página 96

GritonesAplicaciones•template <Class type>  class Ptr { private:       Type* pointee;       static Type* griton; public:       enum ErrorType { accesandoNIL };           Ptr() : pointee(NULL) {}       Ptr(Type* p) : pointee(p) {}       operator Type*() { return pointee; }       Type* operator->()          {           if (pointee == NULL) return griton;           return pointee;        } };  


## Página 97

EstadísticasAplicaciones•template <Class type>  class Ptr { private:       Type* pointee;       static int conversiones;       static int miembros; public:       Ptr() : pointee(NULL) {}       Ptr(Type* p) : pointee(p) {}       operator Type*() { conversiones++; return pointee; }       Type* operator->()          {          miembros++;          return pointee;        } }; •// En Foo.cpp int Ptr<Foo>::conversiones = 0; int Ptr<Foo>::miembros = 0;  


