
#ifndef MYEVENT_H
#define MYEVENT_H

#include <qevent.h>

class MyEvent:public QCustomEvent
{
	public:
		MyEvent(int );

		int value() const;
	private:
		int x;
};

#endif // MYEVENT_H
