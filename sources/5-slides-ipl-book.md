# Chapter 5: Interpreters - Slides

> Documento analizado y transcrito íntegramente preservando el contenido original.

## Página 1

Chapter 5: Interpreters  
Aarne Ranta  
Slides for the book ”Implementing Programming Languages. An Introduction to  
Compilers and Interpreters”, College Publications, 2012.  


## Página 2

The missing piece of a complete language implementation: run your programs and see  
what they produce.  
The code is almost the same as the type checker, thanks to syntax-directed translation.  
Not customary for Java or C, but more so for JavaScript.  
The quickest way to get your language running.  
We will also show an interpreter for the Java Virtual Machine (JVM).  
All the concepts and tools needed for solving Assignment 3za.  


## Página 3

Specifying an interpreter  
Inference rules: operational semantics.  
The rules tell how to evaluate expressions and how to execute statements and whole  
programs.  
The judgement form for expressions is  
γ⊢e⇓v  
Read: expression e evaluates to value v in environment γ.  


## Página 4

Values and environments  
Values can be seen as a special case of expressions, mostly consisting of literals.  
The environment γ(which is a small Γ) assigns values to variables:  
x1:=v1, . . . , x n:=vn  
When evaluating a variable expression, we look up its value from γ:  
γ⊢x⇓vifx:=vinγ  


## Página 5

Example: multiplication  
Evaluation rule:  
γ⊢a⇓u γ ⊢b⇓v  
γ⊢a*b⇓u×v  
where ×is multiplication on the level of values.  
Typing rule:  
Γ⊢a:tΓ⊢b:t  
Γ⊢a*b:t  
One view: typing is a special case of evaluation, where the value is always the type!  


## Página 6

From rule to code  
Pseudocode:  
eval(γ, a*b) :  
u:=eval(γ, a)  
v:=eval(γ, b)  
return u×v  
Haskell:  
eval env (EMul a b) = do  
u <- eval env a  
v <- eval env b  
return (u * v)  


## Página 7

Java:  
public Integer visit (EMul p, Env env) {  
Integer u = eval(p.exp_1, env) ;  
Integer v = eval(p.exp_2, env) ;  
return u * v ;  
}  


## Página 8

Side effects  
Evaluation can have side effects - do things other than return a value.  
•change the environment  
•perform output  
Example: the expression x = 3 on one hand returns the value 3, on the other changes the  
value of x to 3 in the environment.  
The general form of judgement: return a value and a new environment γ′.  
γ⊢e⇓ ⟨v, γ′⟩  
Read: in environment γ, expression e evaluates to value v and to new environment γ′.  


## Página 9

Assignment expressions  
γ⊢e⇓ ⟨v, γ′⟩  
γ⊢x=e⇓ ⟨v, γ′(x:=v)⟩  
The notation γ(x:=v)means that we update the value of xinγtov. This overwrites  
any old value of x.  


## Página 10

Increments  
Operational semantics explains the difference between pre-increments (++x) and post-  
increments (x++).  
γ⊢++x⇓ ⟨v+ 1, γ(x:=v+ 1) ⟩ifx:=vinγ  
γ⊢x++⇓ ⟨v, γ(x:=v+ 1) ⟩ifx:=vinγ  


## Página 11

Propagation of side effects  
Side effects ”contaminate” the rules for all expressions.  
Example: if we start with x:= 1, what is the value of  
x++ - ++x  
Use the interpretation rule for subtraction:  
γ⊢a⇓ ⟨u, γ′⟩γ′⊢b⇓ ⟨v, γ′′⟩  
γ⊢a-b⇓ ⟨u−v, γ′′⟩  


## Página 12

Build a proof tree:  
x:= 1 ⊢x++ ⇓ ⟨1, x:= 2 ⟩x:= 2 ⊢++x ⇓ ⟨3, x:= 3 ⟩  
x:= 1 ⊢x++-++x ⇓ ⟨−2, x:= 3 ⟩  


## Página 13

Statements  
Statements are executed for their side effects, not to receive values.  
Lists of statements are executed in order  
•each statement may change the environment for the next one.  
The evaluation of a sequence  
γ⊢s1. . . s n⇓γ′  
reduces to the interpretation of single statements by the following two rules:  
γ⊢s1⇓γ′γ′⊢s2. . . s n⇓γ′′  
γ⊢s1. . . s n⇓γ′′γ⊢ ⇓ γ(empty sequence)  


## Página 14

Expression statements  
Just ignore the value of the expression:  
γ⊢e⇓ ⟨v, γ′⟩  
γ⊢e;⇓γ′  


## Página 15

If statements  
Difference from the type checker: consider the two possible values of the condition expres-  
sion.  
γ⊢e⇓ ⟨1, γ′⟩γ′⊢s⇓γ′′  
γ⊢if(e)selset⇓γ′′γ⊢e⇓ ⟨0, γ′⟩γ′⊢t⇓γ′′  
γ⊢if(e)selset⇓γ′′  
Exactly one of them matches at run time. Notice the side effect of the condition!  


## Página 16

While statements  
If true, then loop. If false, terminate:  
γ⊢e⇓ ⟨1, γ′⟩γ′⊢s⇓γ′′γ′′⊢while(e)s⇓γ′′′  
γ⊢while(e)s⇓γ′′′γ⊢e⇓ ⟨0, γ′⟩  
γ⊢while(e)s⇓γ′  


## Página 17

Declarations  
Extend the environment with a new variable, which is first given a ”null” value.  
γ⊢t x;⇓γ, x:=null  
Using the null value results in a run-time error, but this is of course impossible to com-  
pletely prevent at compile time (why?).  
We don’t need to check for the freshness of the new variable, because this has been done  
in the type checker!  
(Cf. Milner: ”well-typed programs can’t go wrong”).  


## Página 18

Block statements  
Push a new environment on the stack, just as in the type checker.  
The new variables declared in the block are popped away at exit from the block.  
γ.⊢s1. . . s n⇓γ′.δ  
γ⊢ {s1. . . s n} ⇓γ′  
The new part of the storage, δ, is discarded after the block.  
But the old γpart may have changed values of old variables!  


## Página 19

Example  
{  
int x ; // x := null  
{ // x := null.  
int y ; // x := null. y := null  
y = 3 ; // x := null. y := 3  
x = y + y ; // x := 6. y := 3  
} // x := 6  
x = x + 1 ; // x := 7  
}  


## Página 20

Entire programs  
C convention: the entire program is executed by running its main function, which takes  
no arguments.  
This means the evaluation the expression  
main()  
Which means building a proof tree for  
γ⊢main() ⇓ ⟨v, γ′⟩  
γis the global environment of the program  
•no variables (unless there are global variables)  
•all functions: we can look up any function name fand get the parameter list and the  
function body.  


## Página 21

Function calls  
Execute the body of the function in an environment where the parameters have the values  
of the arguments:  
γ⊢a1⇓ ⟨v1, γ1⟩  
γ1⊢a2⇓ ⟨v2, γ2⟩  
· · ·  
γm−1⊢am⇓ ⟨vm, γm⟩  
γ.x1:=v1, . . . , x m:=vm⊢s1. . . s n⇓ ⟨v, γ′⟩  
γ⊢f(a1, . . . , a n)⇓ ⟨v, γm⟩  
ift f(t1x1, . . . , t mxm){s1. . . , s n}inγ  


## Página 22

This is quite a mouthful. Let us explain it in detail:  
•The first m premisses evaluate the arguments of the function call. As the environment  
can change, we show m versions of γ.  
•The last premiss evaluates the body of the function. This is done in a new environ-  
ment, which binds the parameters of fto its actual arguments.  
•Noothervariablescanbeaccessedwhenevaluatingthebody. Hencethelocalvariables  
in the body won’t be confused with the old variables in γ. Actually, the old variables  
cannot be updated either. All this is already guaranteed by type checking. Thus the  
old environment γis needed here only to look up functions, and using γminstead of  
γhere wouldn’t make any difference.  
•The value that is returned by evaluating the body comes from the return statement  
in the body.  


## Página 23

Return values  
The value of a function body comes from its return statement:  
γ⊢s1. . . s n−1⇓γ′γ′⊢e⇓ ⟨v, γ′′⟩  
γ⊢s1. . . s n−1return e;⇓ ⟨v, γ′′⟩  
If ”well-typed programs can’t go wrong”, the type checker must make sure that function  
bodies have returns.  


## Página 24

Evaluation strategies  
Call by value:  
•arguments are evaluated before the function body is evaluated.  
•followed by functions in C  
Alternative: call by name  
•arguments are inserted into the function body as expressions, before evaluation  
•followed by functions in Haskell (and, in a sense, by macros in C)  
Call by name is also known as lazy evaluation  
•advantage: an argument that is not needed is not evaluated  
•disadvantage: if the variable is used more than once, it has to be evaluated again  
Call by need avoids the disadvantage, and is actually used in Haskell  


## Página 25

Lazy evaluation of boolean expressions  
Also in C and Java: a && b and a ||b are evaluated lazily.  
In a && b, a is evaluated first. If the value is false (0), the whole expression comes out  
false, and b is not evaluated at all.  
This allows the programmer to write  
x != 0 && 2/x > 1  
which would otherwise result in a division-by-zero error when x == 0.  


## Página 26

Semantic of ”and” and ”or”  
Similar to “if” statements: two rules are needed  
γ⊢a⇓ ⟨0, γ′⟩  
γ⊢a&&b⇓ ⟨0, γ′⟩γ⊢a⇓ ⟨1, γ′⟩γ′⊢b⇓ ⟨v, γ′′⟩  
γ⊢a&&b⇓ ⟨v, γ′′⟩  
Similarly for a ||b, where the evaluation stops if a == 1.  


## Página 27

Implementing the interpreter  
Mostly a straightforward variant of the type checker.  
Biggest difference: return types and environment:  
⟨Val,Env⟩eval (Envγ,Expe)  
Env exec (Envγ,Stms)  
Void exec (Program p)  
Val lookup (Ident x,Envγ)  
Def lookup (Ident f,Envγ)  
Env extend (Envγ,Ident x,Valv)  
Env extend (Envγ,Defd)  
Env newBlock (Envγ)  
Env exitBlock (Envγ)  
Env emptyEnv ()  


## Página 28

The top-level interpreter  
First gather the function definitions to the environment, then executes the main function:  
exec(d1. . . d n) :  
γ0:=emptyEnv ()  
fori= 1, . . . , n :  
γi:=extend (γi−1, di)  
eval(γn,main() )  


## Página 29

Statements and expressions  
Easier now, because we don’t have to decide between type checking and type inference!  
Examples:  
exec(γ, e; ) :  
⟨v, γ′⟩:=eval(γ, e)  
return γ′  
exec(γ,while (e)s) :  
⟨v, γ′⟩:=eval(γ, e)  
ifv= 0  
return γ′  
else  
γ′′:=exec(γ′, s)  
exec(γ′′,while (e)s)  


## Página 30

eval(γ, a−b) :  
⟨u, γ′⟩:=eval(γ, a)  
⟨v, γ′′⟩:=eval(γ′, b)  
return ⟨u−v, γ′′⟩  
eval(γ, f(a1, . . . , a m)) :  
fori= 1, . . . , m :  
⟨vi, γi⟩:=eval(γi−1, ai)  
t f(t1x1, . . . , t mxm){s1. . . s n}:=lookup (f, γ)  
⟨v, γ′⟩:=eval(x1:=v1, . . . , x m:=vm, s1. . . s n)  
return ⟨v, γm⟩  


## Página 31

Predefined functions  
In Assignment 3: input and output of reading and printing integers, doubles, and strings  
Implementation: make the eval function, call the host language printing or reading func-  
tions:  
eval(γ,printInt (e)) :  
⟨γ′, v⟩:=eval(γ, e)  
// print integer vto standard output  
return ⟨void-value , γ′⟩  
eval(γ,readInt ()) :  
// read integer vfrom standard input  
return ⟨v, γ⟩  


## Página 32

Defining values in BNFC  
As an algebraic data type - internal, that is, not parsable:  
internal VInteger. Val ::= Integer ;  
internal VDouble. Val ::= Double ;  
internal VString. Val ::= String ;  
internal VVoid. Val ::= ;  
internal VUndefined. Val ::= ;  
You cannot simply write  
VInteger(2) + VInteger(3)  
because + in Haskell and Java is not defined for the type Val.  
Instead, a special function addVal to the effect that  


## Página 33

addVal(VInteger(u),VInteger(v)) = VInteger(u+v)  
addVal(VDouble(u), VDouble(v)) = VDouble(u+v)  
addVal(VString(u), VString(v)) = VString(u+v)  
In Java, + will do for strings, but in Haskell you need ++.  


## Página 34

Implementation in Haskell and Java  
Follow the same structure as in Chapter 4.  
In Haskell, the IO monad is now the most natural choice.  
execStm :: Env -> Stm -> IO Env  
evalExp :: Env -> Exp -> IO (Val,Env)  
In Java, the corresponding types are  
class StmExecuter implements Stm.Visitor<Object,Env> {  
public Object visit(CPP.Absyn.SDecl p, Env env)  
...  
class ExpEvaluator implements Stm.Visitor<Val,Env> {  
public Val visit(CPP.Absyn.EAdd p, Env env)  
...  


## Página 35

The Visitor interface expectedly has the return type Val in ExpEvaluator, and the dummy  
type Object in StmExecuter.  
The environment can be changed as a side effect in Java. In Haskell, this would also be  
possible with a state monad.  


## Página 36

Debugger  
An easy variant of the interpreter.  
Print the environment (i.e. the values of variables) after each change of the values.  
Also print the statement or expression causing the change of the environment.  


## Página 37

Interpreting Java bytecode*  
Java is not an interpreted language!  
Java is compiled to JVM (= Java Virtual Machine =Java bytecode).  
JVM is interpreted.  
The interpreter is much simpler than the one described above.  


## Página 38

Executing a JVM program  
Compiled from the expression 5 + (6 * 7); stack on the right of ”;”  
bipush 5 ; 5  
bipush 6 ; 5 6  
bipush 7 ; 5 6 7  
imul ; 5 42  
iadd ; 47  


## Página 39

JVM instructions  
Machine languages have instructions, instead of expressions and statements. Here are  
some:  
instruction explanation  
bipush n push byte constant n  
iadd pop topmost two values and push their sum  
imul pop topmost two values and push their product  
iload i push value stored in address i  
istore i pop topmost value and store it in address i  
goto L go to code position L  
ifeq L pop top value; if it is 0 go to position L  
The instructions working on integers have variants for other types in the full JVM.  


## Página 40

Variables and addresses  
The code generator assigns an integer memory address to every variable.  
Declarations are compiled so that the next available address is reserved to the variable  
declared.  
Using a variable as an expression means loading it, whereas assigning to it means storing  
it.  


## Página 41

Example Java code and JVM:  
int i ; ; reserve address 0 for i, no code generated  
i = 9 ; bipush 9  
istore 0  
int j = i + 3 ; ; reserve address 1 for j  
iload 0  
bipush 3  
iadd  
istore 1  


## Página 42

Control structures  
E.g. while and if: compiled with jump instructions:  
•goto, unconditional jump,  
•ifeq, conditional jump  
Jumps go to labels, which are positions in the code.  
Example: while statements  
TEST:  
while (exp) ; here, code to evaluate exp  
stm ifeq END  
; here, code to execute stm  
goto TEST  
END:  


## Página 43

Defining a JVM interpreter  
Transitions, a.k.a. small-step semantics: each rule specifies one step of computation.  
The operational semantics for C/Java source code above was big-step semantics: we said  
that a + b is evaluated so that a and b are evaluated first; but each of them can take any  
number of intermediate steps.  
The format of a small-step rule for JVM:  
⟨Instruction ,Env⟩ − → ⟨Env’⟩  
Environment Env:  
•code pointer P,  
•stack S,  
•variable storage V.  


## Página 44

The rules work on instructions, executed one at a time.  
The next instruction is determined by the code pointer.  
Each instruction can do some of the following:  
•increment the code pointer: P+ 1  
•change the code pointer according to a label: P(L)  
•copy a value from a storage address: V(i)  
•write a value in a storage address: V(i:=v)  
•push values on the stack: S.v  
•pop values from the stack  


## Página 45

Semantic rules for some instructions  
⟨bipush v, P, V, S ⟩ − → ⟨ P+ 1, V, S.v⟩  
⟨iadd, P, V, S.v.w ⟩ − → ⟨ P+ 1, V, S.v+w⟩  
⟨imul, P, V, S.v.w ⟩ − → ⟨ P+ 1, V, S.v×w⟩  
⟨iload i, P, V, S ⟩ − → ⟨ P+ 1, V, S.V(i)⟩  
⟨istore i, P, V, S.v ⟩ − → ⟨ P+ 1, V(i:=v), S⟩  
⟨gotoL, P, V, S ⟩ − → ⟨ P(L), V, S⟩  
⟨ifeqL, P, V, S. 0⟩ − → ⟨ P(L), V, S⟩  
⟨ifeqL, P, V, S.v ⟩ − → ⟨ P+ 1, V, S⟩(v̸= 0)  


## Página 46

Relating the two kinds of semantics  
The big-step relation ⇓can be seen as the transitive closure of the small-step relation − →:  
e⇓vmeans that e− →. . .− →vin some number of steps.  
In the JVM case e⇓vmeans that executing the instructions in ereturns the value von  
top of the stack after some number of steps and then terminates.  
This makes it possible, in principle, to prove the correctness of a compiler:  
An expression compiler cis correct if, for all expressions e,e⇓vif and only if  
c(e)⇓v.  


## Página 47

Objects and memory management*  
Values have different sizes: integers might be 32 bits, doubles 64 bits.  
But what is the size of a string?  
It has no fixed size. If you declare a string variable, the size of its value can grow beyond  
any limits when the program is running.  
This is generally the case with objects, which in Java have classes as types.  


## Página 48

Example: a function that replicates a string k times:  
string replicate(int k, string s) {  
string r ;  
r = s ;  
int i = 1 ;  
while (i < k){  
r = s + r ;  
i++ ;  
}  
return r ;  
}  
When the string variable r is declared, one memory word is allocated to store an address.  
Loading r on the stack means loading just this address - which has the same size as an  
integer, independently of the string stored.  
The address indicates the place where the string is stored. It is not on the stack, but in  
another part of the memory, called the heap.  


## Página 49

Run this program with k = 2, s = ”hi”.  
source JVM stack V heap  
; k in 0, s in 1 - 2,&s &s>”hi”  
string r ; ; reserve 2 for r - 2,&s,  
r = s ; aload 1 &s  
astore 2 -2,&s,&s  
int i = 1 ; ; reserve 3 for i - 2,&s,&s,  
bipush 1 1  
istore 3 -2,&s,&s,1  
r = s + r ; aload 1 &s  
aload 2 &s.&s  
; call string-+ &r &s>”hi”,&r >”hihi”  
astore 2 -2,&s,&r,1 &s >”hi”,&r >”hihi”  
The variables in Vare stack variables, and store values of fixed sizes.  
But s and r are heap variables. For them, Vstores just addresses to the heap. They can  
be sharedor or split. Any amount of storage reserved for them may get insuﬀicient.  


## Página 50

Memory management  
When a function terminates (usually by return), its stack storage is freed. But this does  
not automatically free the heap storage.  
For instance, at the exit from the function replicate, the storage for k, s, r, and i is emptied,  
and therefore the addresses &s and &r disappear from V.  
But we cannot just take away the strings ”hi” and ”hihi” from the heap, because they or  
their parts may still be accessed by some other stack variables from outer calls.  
While a program is running, its heap storage can grow beyond all limits, even beyond the  
physical limits of the computer. To prevent this, memory management is needed.  


## Página 51

Manual memory management (C and C++): malloc reserves a part of memory, free makes  
it usable again.  
Standard Template Library (C++) tries to hide much of this from the application pro-  
grammer.  
Garbage collection (Java, Haskell): automatic. A procedure that finds out what parts of  
the heap are still needed (i.e. pointed to from the stack).  


## Página 52

Mark-sweep garbage collection  
Perhaps the simplest of the algorithms.  
The stack is an array of two kinds of elements:  
•data (an integer value)  
•heap address (can also be an integer, but distinguishable from data)  
The heap is segmented to blocks. Each element in the heap is one of:  
•beginning of a block, indicating its length (integer) and freeness (boolean)  
•data (an integer value)  
•heap address (pointing to elsewhere in the heap)  
•unused  
Example: an unaddressed block containing an array of the three integers 6, 7, 8 is a heap  
segment of four elements:  
begin 3 false, data 6, data 7, data 8  


## Página 53

The algorithm composes three functions:  
•roots: find all heap addresses on the stack  
•mark: recursively mark as true all heap blocks that are addressed  
•sweep: replace unmarked memory blocks with ”unused” cells and unmark marked  
blocks  
The boolean freeness value of beginning elements indicates marking in the mark-sweep  
garbage collection. The mark of the beginning node applies to the whole block, so that  
the sweep phase either preserves or frees all nodes of the block.  


