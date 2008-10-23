#ifndef VIEW_H
#define VIEW_H

#include <mydebug.h>

#include <qcanvas.h>

class View:public QCanvasView
{
Q_OBJECT
	public:
		View(QCanvas& canvas,QWidget *parent=0);
		static QCanvasPixmapArray ani;
	protected:
		void dragEnterEvent(QDragEnterEvent *e);
		void dropEvent(QDropEvent *e);
};

#endif // VIEW_H
