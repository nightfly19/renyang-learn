#ifndef CLASSA_H
#define CLASSA_H

#include <qobject.h>

class classA:public QObject
{
	Q_OBJECT
	public:
		classA();
	private:
		int *x;
	signals:
		void Signalvalue(int *);
	public:
		void value();
		void sendsignal();
};

#endif
