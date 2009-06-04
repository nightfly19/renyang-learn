#include <qapplication.h>
#include <qptrlist.h>

#include <iostream>
using namespace std;

int main(int argc,char **argv)
{
	QApplication app(argc,argv);
	QPtrList<int> list;
	
	for (int i=0;i<3;i++)
	{
		list.append(new int(i)); // 插入資料
	}

	list.first(); // 先跳到第一個元素
	for (int i=0;i<3;i++,list.next())
	{
		cout << *(list.current()) << endl;
	}

	list.first();
	if (list.count()>0)
		cout << *(list.current()) << endl;
	else
		qWarning("there is no element");

	list.next();
	if (list.count()>0)
		cout << *(list.current()) << endl;
	else
		qWarning("there is no element");

	list.remove();
	if (list.count()>0)
		cout << *(list.current()) << endl;
	else
		qWarning("there is no element");

	list.remove();
	if (list.count()>0)
		cout << *(list.current()) << endl;
	else
		qWarning("there is no element");

	list.remove();
	if (list.count()>0)
		cout << *(list.current()) << endl;
	else
		qWarning("there is no element");

	// 由這一個例子可以知道,刪掉一個成員後,指標會跑到下一個.但若刪掉後沒有下一個時,指標會跑到前一個

	list.clear();

	return 0;
}
