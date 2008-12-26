#include <iostream>
#include <string>
using namespace std;

string color("red");

void local();

int main()
{
	extern string color;
	cout << "Global color: " << color << endl;
	local();
	return 0;
}

void local()
{
	string color("blue");
	cout << "Local color: " << color << endl;
	cout << "Global color: " << ::color << endl;
}
