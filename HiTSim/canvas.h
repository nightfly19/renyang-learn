#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <qpopupmenu.h>
#include <qmainwindow.h>
#include <qintdict.h>
#include <qcanvas.h>


class EdgeItem;	// because we want use the class EdgeItem, but we still don't know the define of the EdgeItem
class NodeItem;

class EdgeItem: public QCanvasLine
{
public:
    EdgeItem( NodeItem*, NodeItem*, QCanvas *canvas );
    void setFromPoint( int x, int y ) ;
    void setToPoint( int x, int y );
    static int count() { return c; }
    void moveBy(double dx, double dy);
private:
    static int c;
};

class NodeItem: public QCanvasEllipse
{
public:
    NodeItem( QCanvas *canvas );
    ~NodeItem() {}

    void addInEdge( EdgeItem *edge ) { inList.append( edge ); }
    void addOutEdge( EdgeItem *edge ) { outList.append( edge ); }

    void moveBy(double dx, double dy);

    //    QPoint center() { return boundingRect().center(); }
private:
    QPtrList<EdgeItem> inList;
    QPtrList<EdgeItem> outList;
};

#endif

