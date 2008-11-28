#include <iostream> 
using namespace std; 

int foo(); 

int main() { 
    int (*ptr)(void) = 0; // 宣告一個函數指標變數ptr,回傳型別是int,而函式參數是void

    ptr = foo; 

    foo(); 
    ptr(); 

    cout << "address of foo:" 
         << (int)foo << endl; 
    cout << "address of foo:" 
         << (int)ptr << endl; 

    return 0; 
} 

int foo() { 
    cout << "function point" << endl; 

    return 0; 
}
