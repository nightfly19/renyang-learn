#include <iostream>
#include "SafeArray.h"
using namespace std;

int main() {
    //int (SafeArray::*mfPtr1)(int) = 0;
    //void (SafeArray::*mfPtr2)(int, int) = 0;
    typedef void (SafeArray::*MFPTR1)(int, int); 
    typedef int (SafeArray::*MFPTR2)(int);

    MFPTR1 mfPtr1 = 0;
    MFPTR2 mfPtr2 = 0;
 
    mfPtr1 = &SafeArray::set;
    mfPtr2 = &SafeArray::get;
 
    SafeArray safeArray(10);
 
    for(int i = 0; i < safeArray.length; i++) {
        (safeArray.*mfPtr1)(i, (i+1)*10);
    }
 
    for(int i = 0; i < safeArray.length; i++) {
        cout << (safeArray.*mfPtr2)(i) << " ";
    }
    cout << endl;
 
    SafeArray *safePtr = &safeArray;
 
    (safePtr->*mfPtr1)(2, 100);
 
    for(int i = 0; i < safePtr->length; i++) {
        cout << (safePtr->*mfPtr2)(i) << " ";
    }
    cout << endl; 

    return 0;
}
