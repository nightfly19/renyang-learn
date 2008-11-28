/*
 * Name: Ren-Yang Fang
 * Copyright:
 * Author:
 * Date: 28/11/08 18:47
 * Description: 練習如何使用friend函式重載運算子,可以避免左運算子一定要是目前的類別
 */
#include <iostream>
#include "Point2D.h"

using namespace std;

int main(){
	Point2D p1(5,5);
	Point2D p2;

	cout << "p1 = (" << p1.get_x() << ", " << p1.get_y() << ")" << endl;
	cout << "p2 = (" << p2.get_x() << ", " << p2.get_y() << ")" << endl;
	p2 = 10 + p1;
	cout << "after p2 = 10 + p1" << endl;
	cout << "p2 = (" << p2.get_x() << ", " << p2.get_y() << ")" << endl;
	++p2;
	cout << "after ++p2" << endl;
	cout << "p2 = (" << p2.get_x() << ", " << p2.get_y() << ")" << endl;

	cout << "process the p2++" << endl;
	cout << "p2 = (" << p2++.get_x() << ", " << p2.get_y() << ")" << endl;

	cout << "show the p2 again" << endl;
	cout << "p2 = (" << p2.get_x() << ", " << p2.get_y() << ")" << endl;
	return 0;
}
