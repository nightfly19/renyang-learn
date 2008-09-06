
#include <iostream>
using namespace std;

template <class T>
T sum(T a,T b)
{
	return a+b;
}

int main()
{
	int a=1,b=2;
	float c=1.1,d=2.2;

	cout << "The answer of 1 + 2 is " << sum(a,b) << endl;

	cout << "The answer of 1.1 + 2.2 is " << sum(c,d) << endl;

	return 0;
}

