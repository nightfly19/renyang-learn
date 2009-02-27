#include <iostream>
using namespace std;

class foo
{
	public:
		foo() {
			// do nothing
		}

		// 測試是否可以不要回傳參照
		void operator << (const int value) const {
			cout << "good" << endl;
		}
};

int main()
{
	foo x;
	// 會印出good
	x << 2;
	return 0;
}
