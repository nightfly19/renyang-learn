#include <qapplication.h>
#include <qptrlist.h>

#include <iostream>
using namespace std;

int main(int argc,char **argv)
{
	QApplication app(argc,argv);
	QPtrList<int> list;
	int *a[3];
	
	for (int i=0;i<3;i++)
	{
		a[i] = new int(i);
		list.append(a[i]); // 插入資料
	}

	list.first(); // 先跳到第一個元素
	for (int i=0;i<3;i++,list.next())
	{
		cout << *(list.current()) << endl;
	}

	list.first();
	list.next();
	list.remove();
	list.remove();
	list.remove();
	// 由這一個例子可以知道,刪掉一個成員後,指標會跑到下一個.但若刪掉後沒有下一個時,指標會跑到前一個


	// 這一個例子說明由QPtrList中移除, 並不會free掉指標所指到的位置
	printf("\n%d\n",*a[1]);

	return 0;
}
