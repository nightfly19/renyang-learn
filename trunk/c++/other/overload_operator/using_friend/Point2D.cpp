
#include "Point2D.h"

Point2D::Point2D(int x,int y){
	_x = x;
	_y = y;
}

Point2D::Point2D(){
	_x = _y = 0;
}

Point2D operator+(const Point2D &p,int i){
	Point2D tmp(p._x+i,p._y+i);

	return tmp;
}

Point2D operator+(int i,const Point2D &p){
	Point2D tmp(i+p._x,i+p._y);
	
	return tmp;
}

Point2D operator++(Point2D &p){
	p._x++;
	p._y++;

	return p;
}

Point2D operator++(Point2D &p,int){
	Point2D tmp(p._x,p._y);
	p._x++;
	p._y++;

	return tmp;
}
