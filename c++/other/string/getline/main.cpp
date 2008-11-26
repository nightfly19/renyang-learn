#include <iostream>
#include <string>
using namespace std;

int main(){
	string s;
	cout << "Please input a string include the space:";
	getline(cin,s);
	cout << "You typed:" << s << endl;
	s.find("good",0);
	string::size_type loc = s.find("good",0);
	if (loc != string::npos){
		cout << "In the sentence, there is keyword \"good\" "<< endl;
	} else {
		cout << "In the sentence, there is no keyword \"good\"" << endl;
	}
	return 0;
}
