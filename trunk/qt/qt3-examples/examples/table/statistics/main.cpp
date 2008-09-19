/****************************************************************************
** $Id: qt/main.cpp   3.3.7   edited Aug 31 2005 $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "statistics.h"
#include <qapplication.h>
int main( int argc, char **argv )
{
    QApplication a(argc,argv);			

    Table t;
    a.setMainWidget( &t );
    t.show();
    return a.exec();
}
