#include <iostream>
#include "Ball.h"
using namespace std;

int main() {
    Ball b1(10, "RBall");
    Ball b2(20, "GBall");
 
    switch(compare(b1, b2)) {
        case 1:
            cout << b1.name() << " 較大" << endl;
            break;
        case 0:
            cout << b1.name() << " 等於 " << b2.name() << endl;
            break;
        case -1:
            cout << b2.name() << " 較大" << endl;
            break;
    }
 
    return 0;
}
