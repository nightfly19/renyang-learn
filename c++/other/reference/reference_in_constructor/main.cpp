#include <iostream>
using namespace std;

class foo
{
	public:
		// 初始化x成員
		foo(int &value):x(value)
		{
			// do nothing
		}

		// 若以以下方式初始化reference，會出現錯誤
		/*
		foo(int &value)
		{
			x = value;
		}
		*/

		void add()
		{
			x++;
		}

	private:
		int &x;
};

int main()
{
	int y = 2;

	cout << "the original value is " << y << endl;

	// 建立一個物件
	foo *test = new foo(y);

	test->add();

	cout << "the after value is " << y << endl;

	return 0;
}
