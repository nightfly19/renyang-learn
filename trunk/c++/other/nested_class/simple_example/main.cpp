/*
 * Name:
 * Copyright:
 * Author:
 * Date: 28/11/08 12:08
 * Description: 在巢狀類別中，透過friend讓外部類別可以存取內部類別的私有成員
 */
#include <iostream>
using namespace std;

class PointDemo {
	
	public:
		static void print(){
			cout << Point::x << endl;
		}

private:
	
	class Point {
		friend class PointDemo;
		private:
			const static int x=5;
	};

};

int main(){
	cout << "Hello World!!" << endl;
	PointDemo::print();
	return 0;
}
