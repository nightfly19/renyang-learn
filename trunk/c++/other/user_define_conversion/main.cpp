#include <iostream> 
#include "Integer.h"
using namespace std; 

int main() {
    Integer i1(10);
    Integer i2(20);
 
    cout << i1.compareTo(i2) << endl;
 
    cout << i1 + 10 << endl;
 
    return 0;
}
