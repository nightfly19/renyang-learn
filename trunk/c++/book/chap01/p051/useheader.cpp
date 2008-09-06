
#include <iostream>
#include <iomanip>

int main()
{
	float pi = 3.14159;

	cout << "Pi: " << pi << endl;

	cout << setprecision(3) << "Pi with a precision of three digits: " << pi << endl;

	return 0;
}
