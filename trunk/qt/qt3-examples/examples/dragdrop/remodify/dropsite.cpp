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
    setAcceptDrops(TRUE); // 對於這一個視窗元件放開壓住的滑鼠是否產生效果
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
    // action()會傳回Copy,Link,Move,Private;目前drag and drop指的就是這四個動作其中一個
    // acceptAction是用來設定e這一個event是否可以被執行
    // 若目前這一個動作是QDropEvent::Copy的話，表示這一個動作是可以被接受的~
    // e把它想成一個封包被包好的物件
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
	// 若可以被其中一個解碼,就允許drag and drop的動作,也就是以上四個動作之一
    }


    // Give the user some feedback...
    QString t;
    const char *f;
    // 字串是一連串的char,並且以\0作為結尾
    // Returns a string describing one of the available data types for this drag
    // format(n)傳回字串，表示支援的字串指標
    for( int i=0; (f=e->format( i )); i++ ) {
	if ( *(f) ) {
	    if ( !t.isEmpty() ) 
		t += "\n"; // 當原本的t不是空字串時,因為要跳到下一行,在後面加一個跳行
	    t += f; // 加字串只要把指標變數加到QString物件之後
	}
    }
    // 當有拖曳進入DropSite時,會送出message function
    emit message( t );
    setBackgroundColor(white);
}

void DropSite::dragLeaveEvent( QDragLeaveEvent * )
{
    // Give the user some feedback...
    emit message("");
    setBackgroundColor(lightGray);
}


// 當在這一個widget上面拖曳放開之後,會執行這一個function
void DropSite::dropEvent( QDropEvent * e )
{
    setBackgroundColor(lightGray);

    // Try to decode to the data you understand...
    QStrList strings;
    // URL = Uniform Resource Locator
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
	setPixmap( pm ); // 把目前的QLabel的pixmap設定為解出來的pm
	setMinimumSize( minimumSize().expandedTo( sizeHint() ) );
	return;
    }

    if ( SecretDrag::decode( e, str ) ) {
    cout << "SecretDrag" << endl; // debug
	setText( str ); // 把目前的QLabel的text設定為解出來的str
	setMinimumSize( minimumSize().expandedTo( sizeHint() ) );
	return;
    }
}
// 建立一個封裝好的dobj用來拖曳的
DragMoviePlayer::DragMoviePlayer( QDragObject* p ) :
    QObject(p),
    dobj(p),
    movie("trolltech.gif" )
{
    // when an area of the framePixmap() has changed, updatePixmap will be signal
    // 我在猜是把updatePixmap與framePixmap()的change connect 起來
    // 當framePixmap有改變,則呼叫updatePixmap
    // 下面只有當#if 0時才有用到
    movie.connectUpdate(this,SLOT(updatePixmap(const QRect&)));
}

void DragMoviePlayer::updatePixmap( const QRect& )
{
    cout << "DragMoviePlayer" << endl; // debug
    dobj->setPixmap(movie.framePixmap()); // 設定滑鼠動畫
}

void DropSite::mousePressEvent( QMouseEvent * /*e*/ )
{
    cout << "You press the mouse" << endl;
    QDragObject *drobj;
    // pixmap():Returns the label's picture or 0 if the label doesn't have a picture.
    if ( pixmap() ) {
	drobj = new QImageDrag( pixmap()->convertToImage(), this );
	// 把pixmap轉成image
#if 1
// 上面改成0是動畫,改成1是圖片
	QPixmap pm;
	// 把圖變小
	pm.convertFromImage(pixmap()->convertToImage().smoothScale(pixmap()->width()/3,pixmap()->height()/3));
	drobj->setPixmap(pm,QPoint(-5,-7)); // 設定圖片位於滑鼠的哪一個相對位置
#else
	// Try it.
	(void)new DragMoviePlayer(drobj);
#endif
    } else {
	drobj = new QTextDrag( text(), this );
    }
    // 設定這一個拖曳動作是copy滴,一共有四個動作copy,link,move,private
    drobj->dragCopy();
}


void DropSite::backgroundColorChange( const QColor & )
{
    // Reduce flicker by using repaint() rather than update()
    repaint();
}
