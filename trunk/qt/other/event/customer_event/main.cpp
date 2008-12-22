#include <qapplication.h>
#include <iostream>
using namespace std;

#include "mywidget.h"

int main(int argc,char **argv)
{
	QApplication app(argc,argv);

	MyWidget w(0);

	MyEvent *m = new MyEvent(22); // 建立一個事件
	QApplication::postEvent(&w,m); // 傳送事件給w這一個物件
	cout << "傳送事件出去" << endl;

	w.resize(300,300);

	app.setMainWidget(&w);
	w.show();

	return app.exec();
}
