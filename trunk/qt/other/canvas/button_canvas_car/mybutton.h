
#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <qpushbutton.h>

class MyPushButton:public QPushButton{
public:
	MyPushButton(QWidget *parent,const char *name=0,const char *text=0);
};

#endif // MYBUTTON_H

