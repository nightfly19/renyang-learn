#include <iostream> 
#include <string> 
#include <stdexcept> 
using namespace std; 

int main() { 
    try { 
        string str("caterpillar"); 
        cout << "str.substr(20, 2) = " 
             << str.substr(20, 2) 
             << endl; 
    } 
    catch(out_of_range e) { 
        cout << "錯誤: " 
             << e.what() 
             << endl; 
    } 
 
    return 0;
}
