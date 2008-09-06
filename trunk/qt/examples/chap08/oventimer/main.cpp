#include <qapplication.h>

#include "oventimer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    OvenTimer ovenTimer(0);
    ovenTimer.setCaption(QObject::tr("Oven Timer"));
    ovenTimer.resize(200, 200);
    app.setMainWidget(&ovenTimer);
    ovenTimer.show();
    return app.exec();
}
