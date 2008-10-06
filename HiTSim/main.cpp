#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qimage.h>

#include "canvas.h"
#include "MainWindow.h"
#include "BouncyLogo.h"

#include <stdlib.h>

extern QString butterfly_fn;
extern QString logo_fn;

int main(int argc, char** argv)
{
    QApplication app(argc,argv);
    
    srand( time(NULL) );
    
    QCanvas canvas(5048,1300);
    canvas.setAdvancePeriod(30);
    Main m(canvas);
    m.resize(m.sizeHint());

    debug("1");
    if ( QApplication::desktop()->width() > m.width() + 10
	&& QApplication::desktop()->height() > m.height() +30 )
	m.show();
    else
	m.showMaximized();
    
    QObject::connect( qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()) );
    
    return app.exec();
}

