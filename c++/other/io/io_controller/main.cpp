#include <iostream> 
#include <iomanip> 
using namespace std; 
                                                                                
int main() {
    cout << oct << 50 << endl    // 8進位顯示 
         << hex << 50 << endl;   // 16進位顯示 
                                                                                
    // 九九乘法表 
    for(int j = 1; j < 10; j++) { 
       for(int i = 2; i < 10; i++) { 
           cout << i << "*" << j << "=" << dec << setw(2) << i*j; 
           cout << "  "; 
       } 
       cout << endl; 
    }
     
    return 0; 
}
