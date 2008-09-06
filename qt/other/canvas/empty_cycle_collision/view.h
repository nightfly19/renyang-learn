#ifndef VIEW_H
#define VIEW_H

#include <qcanvas.h>

class View:public QCanvasView
{
Q_OBJECT
	public:
		View(QCanvas& canvas);
		QCanvasRectangle* getPolygon() const;
		QCanvasRectangle* getRectangle() const;
		QCanvasText* getText() const;

	protected:
		void contentsMousePressEvent(QMouseEvent *e);
	
	private:
		QCanvasRectangle *p;
		QCanvasRectangle *r;
		QCanvasText *t;
};

#endif // VIEW_H
