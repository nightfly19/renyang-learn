#ifndef VIEW_H
#define VIEW_H

#include <qcanvas.h>

class View:public QCanvasView
{
Q_OBJECT
	public:
		View(QCanvas& canvas);
		QCanvasRectangle* getPolygon() const;
		QCanvasRectangle* getPolygon1() const;
		QCanvasRectangle* getRectangle() const;
		QCanvasText* getText() const;
		QCanvasRectangle *Rectangle(int x,int y,int size) const;

	protected:
		void contentsMousePressEvent(QMouseEvent *e);
	
	private:
		void empty_Rectangle(int x,int y,int size,int thickness=1);
		QCanvasRectangle *p,*p1;
		QCanvasRectangle *r;
		QCanvasText *t;
};

#endif // VIEW_H
