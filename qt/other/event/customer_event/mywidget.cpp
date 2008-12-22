
#include "mywidget.h"

#include <iostream>
using namespace std;

MyWidget::MyWidget(QWidget *parent,const char *name):QWidget(parent,name)
{
	// do nothing
}

void MyWidget::customEvent(QCustomEvent *e){
	if (e->type() == 346798) {
		MyEvent *ce = (MyEvent *) e; // 透過父類別的子標變數接收,要強制轉換成子類別的子標
		cout << "接收到事件,EventID=" << ce->value() << endl;
	}
}
