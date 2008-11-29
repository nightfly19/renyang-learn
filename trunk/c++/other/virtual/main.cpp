
#include <iostream>
using namespace std;

class Foo1
{
	public:
		virtual void show()
		{
			cout << "Foo1's show" << endl;
		}
};

class Foo2:public Foo1
{
	public:
		virtual void show() // 這一行的virtual可加可不加,因為,沒有子類別要修改這一個function了
		{
			cout << "Foo2's show" << endl;
		}
};

void showFooByPtr(Foo1 *foo)
{
	foo->show();
}

void showFooByRef(Foo1 &foo)
{
	foo.show();
}

int main()
{
	Foo1 f1;
	Foo2 f2;

	showFooByPtr(&f1);
	showFooByPtr(&f2);
	cout << endl;

	showFooByRef(f1);
	showFooByRef(f2);
	cout << endl;

	f1.show();
	f2.show();

	return 0;
}

