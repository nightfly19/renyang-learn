#include <iostream>
#include "PointDemo.h"
#include "Point.h"
using namespace std;

PointDemo::PointDemo(int length) : _length(length) {
    _points = new Point*[_length];
    for(int i = 0; i < _length; i++) { 
        _points[i] = new Point(); // 這一個陣列內含值是Point物件的指標，因此，這一個陣列的型別是(Point*)，而指到這一個陣列的變數應該是*(Point*)=(Point **)
        _points[i]->x(i*5);
        _points[i]->y(i*5);
    } 
}

void PointDemo::show() {
    for(int i = 0; i < _length; i++) { 
        cout << "(x, y) = (" 
             << _points[i]->x() << ", "
             << _points[i]->y() << ")"
             << endl;
    } 
}

PointDemo::~PointDemo() {
    for(int i = 0; i < _length; i++) { 
        delete _points[i];
    } 
    delete [] _points;
}
