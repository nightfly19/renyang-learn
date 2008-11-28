#include <iostream> 
#include <cstdarg> 
using namespace std; 

void foo(int, ...); 

int main() { 
    double x = 1.1, y = 2.1, z = 3.9; 
    double a = 0.1, b = 0.2, c = 0.3; 

    cout << "三個參數：" << endl; 
    foo(3, x, y, z); 

    cout << "六個參數：" << endl; 
    foo(6, x, y, z, a, b, c); 

    return 0; 
} 

void foo(int i, ...) { 
    double tmp; 
    va_list num_list; 

    va_start(num_list, i); 

    for(int j = 0; j < i; j++) 
        cout << va_arg(num_list, double) << endl; 

    va_end(num_list); 
}
