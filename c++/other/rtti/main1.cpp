
#include <iostream> 
#include <typeinfo> 
using namespace std; 

class Base { 
public: 
    virtual void foo() = 0;
}; 

class Derived1 : public Base { 
public: 
    void foo() { 
        cout << "Derived1" << endl; 
    } 
 
    void showOne() {
        cout << "Yes! It's Derived1." << endl;
    }
}; 

class Derived2 : public Base { 
public: 
    void foo() { 
        cout << "Derived2" << endl; 
    } 
 
    void showTwo() {
        cout << "Yes! It's Derived2." << endl;
    }
}; 

void showWho(Base *base) {
    base->foo();
 
    if(typeid(*base) == typeid(Derived1)) {
        Derived1 *derived1 = static_cast<Derived1*>(base);
        derived1->showOne();
    }
    else if(typeid(*base) == typeid(Derived2)) {
        Derived2 *derived2 = static_cast<Derived2*>(base);
        derived2->showTwo(); 
    } 
}

int main() { 
    Derived1 derived1;
    Derived2 derived2; 

    showWho(&derived1);
    showWho(&derived2);
 
    return 0;
}
