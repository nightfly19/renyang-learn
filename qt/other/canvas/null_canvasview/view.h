#ifndef VIEW_H
#define VIEW_H

#include <mydebug.h>

#include <qcanvas.h>

class View:public QCanvasView
{
Q_OBJECT
	public:
		View(QCanvas& canvas);
};

#endif // VIEW_H
