#include "listboxcombo.h"
#include <qapplication.h>

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    ListBoxCombo listboxcombo;
    listboxcombo.resize( 400, 270 );
    listboxcombo.setCaption( "Qt Example - Listboxes and Comboboxes" );
    a.setMainWidget( &listboxcombo );
    listboxcombo.show();

    return a.exec();
}
