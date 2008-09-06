
#include <iostream>

using namespace std;

enum Food{first=2,second,third};

int main()
{
	Food test;

	test = Food(3);

	cout << test << endl;

	return 0;
}
