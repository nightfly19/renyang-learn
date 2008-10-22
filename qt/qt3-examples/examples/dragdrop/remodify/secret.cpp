/****************************************************************************
** $Id: qt/secret.cpp   3.3.7   edited May 27 2003 $
**
** Custom MIME type implementation example
**
** Created : 979899
**
** Copyright (C) 1997 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "secret.h"
#include <qevent.h>

#include <iostream>
using namespace std;


//create the object withe the secret byte
SecretDrag::SecretDrag( uchar secret, QWidget * parent, const char * name )
    : QStoredDrag( "secret/magic", parent, name )
{
    cout << "secret is " << secret << endl;
    QByteArray data(1);
    data[0]= secret;
    setEncodedData( data ); // 設定可以支援的類型,我在猜,可能要解"secret/magic"必需要用一連串的QByteArray當key
    // Sets the encoded data of this drag object to encodedData. The encoded data is what's delivered to the drop sites. It must be in a strictly defined and portable format.
    // The drag object can't be dropped (by the user) until this function has been called.
    // 只有當這一個定義好之後,在這一個site拖曳才會有東西
}


bool SecretDrag::canDecode( QDragMoveEvent* e )
{
    return e->provides( "secret/magic" );
    // 判斷是否有支援"secret/magic"
}

//decode it into a string
bool SecretDrag::decode( QDropEvent* e, QString& str )
{
    QByteArray payload = e->encodedData( "secret/magic" ); // 傳回這一個類型解碼key是啥
    if ( payload.size() ) {
	e->accept(); // 因為可以解碼(payload.size()!=0),所以目前的拖曳是被允許的
	QString msg;
	msg.sprintf("The secret number is %d", payload[0] ); // 傳回格式化字串,把數字轉成字串
	str = msg;
	return TRUE;
    }
    return FALSE;
}


// 設定secret這一個顯示的格式,建構子
SecretSource::SecretSource( int secret, QWidget *parent, const char * name )
    : QLabel( "Secret", parent, name )
{
    setBackgroundColor( blue.light() );
    setFrameStyle( Box | Sunken );
    setMinimumHeight( sizeHint().height()*2 );
    setAlignment( AlignCenter );
    mySecret = secret;
}

SecretSource::~SecretSource()
{
}

/* XPM */
static const char * picture_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #000000",
"X	c #FFFF00",
"     .....      ",
"   ..XXXXX..    ",
"  .XXXXXXXXX.   ",
" .XXXXXXXXXXX.  ",
" .XX..XXX..XX.  ",
".XXXXXXXXXXXXX. ",
".XX...XXX...XX. ",
".XXX..XXX..XXX. ",
".XXXXXXXXXXXXX. ",
".XXXXXX.XXXXXX. ",
" .XX.XX.XX.XX.  ",
" .XXX..X..XXX.  ",
"  .XXXXXXXXX.   ",
"   ..XXXXX..    ",
"     .....      ",
"                "};

// 只有當壓下去時，之後才會有拖曳的動作啊
void SecretSource::mousePressEvent( QMouseEvent * /*e*/ )
{
    // 建立一個拖曳物件
    SecretDrag *sd = new SecretDrag( mySecret, this );
    cout << "mySecret is " << mySecret << endl; // debug
    sd->setPixmap(QPixmap(picture_xpm),QPoint(8,8));
    sd->dragCopy(); // 這一個拖曳的動作是copy
    mySecret++;
}
