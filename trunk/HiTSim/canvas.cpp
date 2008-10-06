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

#include "canvas.h"
#include "ImageItem.h"
#include "global.h"

#include <stdlib.h>


// We use a global variable to save memory - all the brushes and pens in
// the mesh are shared.
static QBrush *tb = 0;
static QPen *tp = 0;

int EdgeItem::c = 0;


void EdgeItem::moveBy(double, double)
{
    //nothing
}

EdgeItem::EdgeItem( NodeItem *from, NodeItem *to, QCanvas *canvas )
    : QCanvasLine( canvas )
{
    c++;
    setPen( *tp );
    setBrush( *tb );
    from->addOutEdge( this );
    to->addInEdge( this );
    setPoints( int(from->x()), int(from->y()), int(to->x()), int(to->y()) );
    setZ( 127 );
}

void EdgeItem::setFromPoint( int x, int y )
{
    setPoints( x,y, endPoint().x(), endPoint().y() );
}

void EdgeItem::setToPoint( int x, int y )
{
    setPoints( startPoint().x(), startPoint().y(), x, y );
}



void NodeItem::moveBy(double dx, double dy)
{
    QCanvasEllipse::moveBy( dx, dy );

    QPtrListIterator<EdgeItem> it1( inList );
    EdgeItem *edge;
    while (( edge = it1.current() )) {
	++it1;
	edge->setToPoint( int(x()), int(y()) );
    }
    QPtrListIterator<EdgeItem> it2( outList );
    while (( edge = it2.current() )) {
	++it2;
	edge->setFromPoint( int(x()), int(y()) );
    }
}

NodeItem::NodeItem( QCanvas *canvas )
    : QCanvasEllipse( 6, 6, canvas )
{
    setPen( *tp );
    setBrush( *tb );
    setZ( 128 );
}
