#include <memory>
#include <iostream>
using namespace std;

int main(){
	auto_ptr<int> iPtr (new int(100));
	auto_ptr<string> sPtr(new string("caterpillar"));

	cout << *iPtr << endl;
	if (sPtr->empty())
		cout << "字串為空" << endl;
	
	if (iPtr.get()==0)
		cout << "沒有資料" << endl;
	else
		cout << "有資料" << endl;
	
	return 0;
}
