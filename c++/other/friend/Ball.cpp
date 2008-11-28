#include "Ball.h"

// compare 為 Ball 的 friend 
int compare(Ball &b1, Ball &b2) {
    // 可直接存取私用成員
    if(b1._radius == b2._radius)
        return 0;
    else if(b1._radius > b2._radius)
        return 1;
    else
        return -1;
}

Ball::Ball(double radius, char *name) { 
    _radius = radius; 
    _name = name;
}
