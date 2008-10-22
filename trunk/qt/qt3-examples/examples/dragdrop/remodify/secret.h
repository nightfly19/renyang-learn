/****************************************************************************
** $Id: qt/secret.h   3.3.7   edited May 27 2003 $
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

#ifndef SECRETDRAG_H
#define SECRETDRAG_H

#include <qdragobject.h>
#include <qlabel.h>

// 定義出要拖曳的滑鼠要帶的物件
class SecretDrag: public QStoredDrag {
public:
    SecretDrag( unsigned char, QWidget * parent = 0, const char * name = 0 );
    ~SecretDrag() {};

    // 加入可以解碼的string
    static bool canDecode( QDragMoveEvent* e );
    // 把dragdrop object 解出到QString
    static bool decode( QDropEvent* e, QString& s );
};


// 設定好Secret Source的版面
class SecretSource: public QLabel
{
public:
    SecretSource( int secret, QWidget *parent = 0, const char * name = 0 );
    ~SecretSource();

protected:
    void mousePressEvent( QMouseEvent * );
private:
    int mySecret;
};

#endif
