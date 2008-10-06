#include <qdatetime.h>
#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qlabel.h>
#include <qimage.h>
#include <qprogressdialog.h>

#include "FigureEditor.h"
#include "ImageItem.h"
#include "global.h"
#include "Car.h"

#include <stdlib.h>

FigureEditor::FigureEditor(
	QCanvas& c, QWidget* parent,
	const char* name, WFlags f) :
    QCanvasView(&c,parent,name,f)
{
	// do nothing
}

void FigureEditor::contentsMousePressEvent(QMouseEvent* e)
{
    QPoint p = inverseWorldMatrix().map(e->pos());
    QCanvasItemList l=canvas()->collisions(p);

    if( e ->button() == Qt::LeftButton ){
	    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
		    if ( (*it)->rtti() == imageRTTI ) {
			    ImageItem *item= (ImageItem*)(*it);
			    if ( !item->hit( p ) )
				    continue;
		    }
		    if ( (*it)->rtti() == ROAD_RTTI ) {
			continue;
		    }
		    moving = *it;
		    moving_start = p;
		    
		    emit leftSelect(*it);
		    return;
	    }
    } else if( e ->button() == Qt::RightButton ){
	    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
		    if ( (*it)->rtti() == CAR_RTTI) {
			    //Car* item= (Car*)(*it);

			    emit rightSelect(*it);
			    debug("right select some car");
		    }
		    return;
	    } 
    } 


    moving = 0;
}

void FigureEditor::contentsMouseReleaseEvent(QMouseEvent* )
{
	emit sigMouseRelease();
}

void FigureEditor::clear()
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
	if ( *it )
	    delete *it;
    }
}

void FigureEditor::contentsMouseMoveEvent(QMouseEvent* e)
{
    if ( moving ) {
	QPoint p = inverseWorldMatrix().map(e->pos());
	moving->moveBy(p.x() - moving_start.x(),
		       p.y() - moving_start.y());
	moving_start = p;
	canvas()->update();
    }
}
