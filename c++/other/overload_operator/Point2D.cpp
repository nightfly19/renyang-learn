#include "Point2D.h"

Point2D::Point2D() {
    _x = 0;
    _y = 0;
}

Point2D::Point2D(int x, int y) {
    _x = x;
    _y = y;
}

Point2D Point2D::operator+(const Point2D &p) { 
    Point2D tmp(_x + p._x, _y + p._y); 
    return tmp; 
} 

Point2D Point2D::operator-(const Point2D &p) { 
    Point2D tmp(_x - p._x, _y - p._y); 
    return tmp; 
} 

Point2D& Point2D::operator++() { 
    _x++; 
    _y++; 

    return *this; 
} 

Point2D Point2D::operator++(int) { 
    Point2D tmp(_x, _y); 
    _x++; 
    _y++; 

    return tmp; 
} 

Point2D& Point2D::operator--() { 
    _x--; 
    _y--; 

    return *this; 
} 

Point2D Point2D::operator--(int) { 
    Point2D tmp(_x, _y); 
    _x--; 
    _y--; 

    return tmp; 
}
