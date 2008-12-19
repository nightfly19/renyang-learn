#include "lineedits.h"
#include <qapplication.h>

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    LineEdits lineedits;
    lineedits.setCaption( "Qt Example - Lineedits" );
    a.setMainWidget( &lineedits );
    lineedits.show();

    return a.exec();
}
