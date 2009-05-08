#ifndef CLASSB_H
#define CLASSB_H

#include <qobject.h>

class classB:public QObject
{
	Q_OBJECT
	public:
		classB();
		int *x;
	public slots:
		void Slotvalue(int *);
};

#endif
