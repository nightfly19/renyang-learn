
#include <iostream>
#include <string>
using namespace std;

class bird_class
{
	public:
		string name;
		int number_seen;
		void increment_count(){number_seen++;}
		bird_class(string text);
		const bird_class & compare(const bird_class & other_bird);
};

bird_class::bird_class(string text)
{
	name = text;
	number_seen = 0;
}

const bird_class & bird_class::compare(const bird_class & other_bird)
{
	if (number_seen > other_bird.number_seen)
		return *this;
	else
		return other_bird;
}

int main()
{
	bird_class orioles("oriole");
	bird_class robins("robin");

	orioles.increment_count();

	cout << "Type of most birds seen: " << orioles.compare(robins).name << endl;

	return 0;
}
