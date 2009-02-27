#include <iostream>
#include <string>
using namespace std;
class product
{
		int number;
	public:
		string name;
		int get_number() {
			return number;
		}
		void set_number(int n) {
			number=n;
		}
		product (string text,int count);
		friend ostream &operator<<(ostream &stream,const product &p);
};

product::product(string text,int count)
{
	name = text;
	number = count;
}

ostream &operator<<(ostream & stream,const product &p)
{
	stream << "Number of " << p.name << ": " << p.number;
	// 似乎不需要回傳stream也沒有關係
	return stream;
}

int main()
{
	product oranges("oranges",200);
	cout << oranges << endl;
	return 0;
}
