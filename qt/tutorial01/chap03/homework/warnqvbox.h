
#ifndef WARNQVBOX_H
#define WARNQVBOX_H

#include <qvbox.h>

class warnQVBox:public QVBox
{
	Q_OBJECT
	public slots:
		void warnmessage();

};

#endif // WARNQVBOX_H
