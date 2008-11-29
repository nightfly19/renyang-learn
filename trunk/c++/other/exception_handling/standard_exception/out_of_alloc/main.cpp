#include <iostream> 
#include <stdexcept> 
using namespace std; 
 
int main() { 
    double *ptr; 
 
    do { 
        try { 
            ptr = new double[1000000]; 
        } 
        catch(bad_alloc err) { 
            cout << "配置失敗" 
                 << err.what() 
                 << endl; 
             return 1; 
        } 
        cout << "配置OK\n"; 
    } while(ptr); 
 
    return 0;
}
