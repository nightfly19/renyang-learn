#include <iostream>
using namespace std;

class bbb
{
public:
	bbb();
	~bbb();
};

bbb::bbb()
{

}

bbb::~bbb()
{
	cout << "destruct" << endl;
}

int main()
{
	bbb b;
	// exit(0);
	return 0;
}
