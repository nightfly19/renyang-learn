
#include <iostream>
using namespace std;

enum Action {stop=3, sit=5, stand=7, walk=9, run=11};

int main()
{
	Action action = stop;

	switch(action)
	{
		case stop:
			cout << "that is stop" << endl;
			break;
		case sit:
			cout << "that is sit" << endl;
			break;
		case stand:
			cout << "that is stand" << endl;
			break;
		case walk:
			cout << "that is walk" << endl;
			break;
		case run:
			cout << "that is run" << endl;
			break;
		default:
			cout << "otherwise" << endl;
	}

	cout << "the number present the variout is " << action << endl;

	return 0;
}

