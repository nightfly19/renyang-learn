
#include <iostream>
using namespace std;

int main(){
	int number=int();
	cout << "Please input a integer:";
	cin >> number;
	switch(number){
		case 1 ... 30:
			cout << "The number is between 1 and 30" << endl;
			break;
		case 31 ... 1000:
			cout << "The number is between 31 and 1000" << endl;
			break;
		default:
			cout << "The number is not between 1 and 1000" << endl;
	}
	return 0;
}
