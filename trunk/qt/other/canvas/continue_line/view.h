#ifndef VIEW_H
#define VIEW_H

#include <qcanvas.h>

class View:public QCanvasView
{
Q_OBJECT
	public:
		View(QCanvas& canvas);

	protected:
		void contentsMousePressEvent(QMouseEvent *e);
	
	private:
		QPoint *start,*end;
		bool started;
};

#endif // VIEW_H
