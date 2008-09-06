
#include <iostream>
#include <string>
using namespace std;
#include "birdclassheader"

int main()
{
	bird_class orioles("oriole");

	orioles.increment_count();

	cout << "Number of " << orioles.name << "s seen: " << orioles.number_seen <<endl;

	return 0;
}
