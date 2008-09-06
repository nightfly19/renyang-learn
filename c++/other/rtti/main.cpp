
#include <iostream> 
#include <typeinfo> 
using namespace std; 

class Base { 
public: 
    virtual void foo() { 
        cout << "Base" << endl; 
    } 
}; 

class Derived1 : public Base { 
public: 
    void foo() { 
        cout << "Derived1" << endl; 
    } 
}; 

class Derived2 : public Base { 
public: 
    void foo() { 
        cout << "Derived2" << endl; 
    } 
}; 

int main() { 
    Base *ptr; // 基底類別指標 
    Base base; 
    Derived1 derived1;
    Derived2 derived2; 

    ptr = &base;
    cout << "ptr 指向 " 
         << typeid(*ptr).name() 
         << endl; 

    ptr = &derived1; 
    cout << "ptr 指向 " 
         << typeid(*ptr).name() 
         << endl; 

    ptr = &derived2; 
    cout << "ptr 指向 " 
         << typeid(*ptr).name() 
         << endl;
 
    return 0;
}
