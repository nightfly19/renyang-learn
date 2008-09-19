/****************************************************************************
** $Id: qt/main.cpp   3.3.7   edited Aug 31 2005 $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include <qsqldatabase.h>
#include <qsqlcursor.h>
#include "../connection.h"

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv, FALSE );

    if ( createConnections() ) {
   	QSqlCursor cur( "staff" );
	QStringList fields = QStringList() << "id" << "forename";
	QSqlIndex order = cur.index( fields );
	QSqlIndex filter = cur.index( "surname" );
	cur.setValue( "surname", "Bloggs" );
	cur.select( filter, order ); 
	while ( cur.next() ) {
	    qDebug( cur.value( "id" ).toString() + ": " +
		    cur.value( "surname" ).toString() + " " +
		    cur.value( "forename" ).toString() );
        }
    }

    return 0;
}
