#include "Integer.h"

int Integer::compareTo(Integer integer) {
    if(_value > integer._value) {
        return 1;
    }
    else if(_value < integer._value) {
        return -1;
    }
 
    return 0;
}
