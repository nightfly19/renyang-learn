// 測試回傳值後面可以加判斷式

#include <iostream>
using namespace std;

int value(int x)
{
	return x==0;
}

int main()
{
	cout << value(1) << endl;

	cout << value(0) << endl;

	cout << value(2) << endl;

	return 0;
}
