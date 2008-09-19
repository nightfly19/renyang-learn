
#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <qvbox.h>

class QLCDNumber;

class MyWidget:public QVBox
{
	Q_OBJECT
		public:
			MyWidget(QWidget *parent=0,const char *name=0);

		public slots:
			void change2hex();
			void change2dec();
			void change2oct();
			void change2bin();

		private:
			QLCDNumber *lcd;
};

#endif // MYWIDGET_H

