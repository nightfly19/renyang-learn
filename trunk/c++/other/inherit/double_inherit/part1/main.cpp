
#include <iostream> 
using namespace std; 

class FooA { 
public: 
    FooA() { 
        cout << "執行FooA建構函式" << endl;
    } 

    ~FooA() { 
        cout << "執行FooA解構函式" << endl;
    } 
}; 

class FooB { 
public: 
    FooB() { 
        cout << "執行FooB建構函式" << endl;
    } 

    ~FooB() { 
        cout << "執行FooB解構函式" << endl;
    } 
}; 

class FooC : public FooA, public FooB { 
public: 
    FooC() { 
        cout << "執行FooC建構函式" << endl;
    } 

    ~FooC() { 
        cout << "執行FooC解構函式" << endl;
    } 
}; 

int main() { 
    FooC c; 

    cout << endl; 

    return 0;
}

