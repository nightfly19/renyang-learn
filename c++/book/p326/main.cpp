
#include <iostream>
#include <string>
using namespace std;

struct bird_structure
{
	string name;
	int number_seen;
};

int main()
{
	bird_structure orioles;

	orioles.name = "oriole";
	orioles.number_seen = 0;

	orioles.number_seen++;

	cout << "Number of " << orioles.name << "s seen: " << orioles.number_seen << endl;

	return 0;
}
