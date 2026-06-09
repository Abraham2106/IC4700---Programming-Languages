# Guide for CPP Functions 


## Section (name pending)

### Virtual 

A **virtual function** is a member function in the base class that can be **overriden** in derived classes. 
**Virtual functions** are a key part of *polymorphism* in C++. 
They let different objects responde differently to the same function call. 

#### Use case 

Without *virtual*, C++ decides which function to call based on the pointer type, not just the actual type. 

With *virtual*, it checks the actual object the pointer is pointing to. 

Or more simply:
* **Without** : the base functions runs, even if the object is from a child class. 
* **With** : the child´s version runs. 


#### Code example 
```cpp
class Animal {
    public: 
        void sound() {
            cout << "Sonido\n"; 
        }

};


class Dog : public Animal {
    public: 
        void sound() {
            cout << "Dog Sound\n"
        }
};


int main() {
    Animal* a; // Declare pointer to the base class (Animal)
    Dog d; 
    a = &d;
    a->sound();
    return 0; 
}
```