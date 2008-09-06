
#include <qapplication.h>
#include <qpushbutton.h>
#include <qfont.h>

#include "warnqvbox.h"

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    warnQVBox box;
    box.resize( 200, 120 );

    QPushButton quit( "Quit", &box );
    quit.setFont( QFont( "Times", 18, QFont::Bold ) );

    QPushButton show( "Show", &box );
    show.setFont(QFont("Times",18,QFont::Bold));

    QObject::connect( &quit, SIGNAL(clicked()), &a, SLOT(quit()) );

    QObject::connect( &show,SIGNAL(clicked()), &box,SLOT(warnmessage()));

    a.setMainWidget( &box );
    box.show();

    return a.exec();
}

