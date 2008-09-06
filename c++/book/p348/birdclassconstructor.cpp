
#include <iostream>
#include <string>
using namespace std;

class bird_class
{
	public:
		string *name;
		int number_seen;
		void increment_count(){number_seen++;}
		bird_class(string text);
		~bird_class();
};

bird_class::bird_class(string text)
{
	name = new string(text);
	number_seen = 0;
}

bird_class::~bird_class()
{
	delete name;
}

int main()
{
	bird_class orioles("oriole");

	orioles.increment_count();

	cout << "Number of " << *(orioles.name) << "s seen: " << orioles.number_seen <<endl;

	return 0;
}
