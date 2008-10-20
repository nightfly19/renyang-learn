/****************************************************************************
** $Id: qt/dropsite.cpp   3.3.7   edited Dec 3 2003 $
**
** Drop site example implementation
**
** Created : 979899
**
** Copyright (C) 1997 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "dropsite.h"
#include "secret.h"
#include <qevent.h>
#include <qpixmap.h>
#include <qdragobject.h>
#include <qimage.h>
#include <qdir.h>

#include <iostream>
using namespace std;


DropSite::DropSite( QWidget * parent, const char * name )
    : QLabel( parent, name )
{
    setAcceptDrops(TRUE); // 允許在這一個widget上可以使用拖曳
}


DropSite::~DropSite()
{
    // nothing necessary
}


void DropSite::dragMoveEvent( QDragMoveEvent *e )
{
    // Check if you want the drag at e->pos()...
    // Give the user some feedback - only copy is possible
    e->acceptAction( e->action() == QDropEvent::Copy );
    // 若目前這一個動作是QDropEvent::Copy的話，表示這一個動作是可以被接受的~
}


void DropSite::dragEnterEvent( QDragEnterEvent *e )
{
    // Check if you want the drag...
    if ( SecretDrag::canDecode( e ) // 當有支援"secret/magic",傳回true
      || QTextDrag::canDecode( e )  // 是否可被解碼為QString
      || QImageDrag::canDecode( e ) // Returns TRUE if the information in mime source e can be decoded into an image
      || QUriDrag::canDecode( e ) ) // 是否可被解碼
    {
	e->accept();
    }


    // Give the user some feedback...
    QString t;
    const char *f;
    // Returns a string describing one of the available data types for this drag
    for( int i=0; (f=e->format( i )); i++ ) {
	if ( *(f) ) {
	    if ( !t.isEmpty() )
		t += "\n"; // 當原本的t不是空字串時，在後面加一個跳行
	    t += f;
	}
    }
    emit message( t );
    setBackgroundColor(white);
}

void DropSite::dragLeaveEvent( QDragLeaveEvent * )
{
    // Give the user some feedback...
    emit message("");
    setBackgroundColor(lightGray);
}


void DropSite::dropEvent( QDropEvent * e )
{
    setBackgroundColor(lightGray);

    // Try to decode to the data you understand...
    QStrList strings;
    if ( QUriDrag::decode( e, strings ) ) {
    cout << "QUriDrag" << endl; // debug
	QString m("Full URLs:\n");
	for (const char* u=strings.first(); u; u=strings.next())
	    m = m + "   " + u + '\n';
	QStringList files;
	if ( QUriDrag::decodeLocalFiles( e, files ) ) {
	    m += "Files:\n";
	    for (QStringList::Iterator i=files.begin(); i!=files.end(); ++i)
		m = m + "   " + QDir::convertSeparators(*i) + '\n';
	}
	setText( m );
	setMinimumSize( minimumSize().expandedTo( sizeHint() ) );
	return;
    }

    QString str;
    if ( QTextDrag::decode( e, str ) ) {
    cout << "QTextDrag" << endl;  // debug
	setText( str );
	setMinimumSize( minimumSize().expandedTo( sizeHint() ) );
	return;
    }

    QPixmap pm;
    if ( QImageDrag::decode( e, pm ) ) {
    cout << "QImageDrag" << endl; // debug
	setPixmap( pm );
	setMinimumSize( minimumSize().expandedTo( sizeHint() ) );
	return;
    }

    if ( SecretDrag::decode( e, str ) ) {
    cout << "SecretDrag" << endl; // debug
	setText( str );
	setMinimumSize( minimumSize().expandedTo( sizeHint() ) );
	return;
    }
}

DragMoviePlayer::DragMoviePlayer( QDragObject* p ) :
    QObject(p),
    dobj(p),
    movie("trolltech.gif" )
{
    movie.connectUpdate(this,SLOT(updatePixmap(const QRect&)));
}

void DragMoviePlayer::updatePixmap( const QRect& )
{
    dobj->setPixmap(movie.framePixmap());
}

void DropSite::mousePressEvent( QMouseEvent * /*e*/ )
{
    QDragObject *drobj;
    if ( pixmap() ) {
	drobj = new QImageDrag( pixmap()->convertToImage(), this );
#if 1
	QPixmap pm;
	pm.convertFromImage(pixmap()->convertToImage().smoothScale(
	    pixmap()->width()/3,pixmap()->height()/3));
	drobj->setPixmap(pm,QPoint(-5,-7));
#else
	// Try it.
	(void)new DragMoviePlayer(drobj);
#endif
    } else {
	drobj = new QTextDrag( text(), this );
    }
    drobj->dragCopy();
}


void DropSite::backgroundColorChange( const QColor & )
{
    // Reduce flicker by using repaint() rather than update()
    repaint();
}
