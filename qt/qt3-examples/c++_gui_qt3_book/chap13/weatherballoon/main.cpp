#include <qapplication.h>

#include "weatherballoon.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    WeatherBalloon balloon;
    balloon.setCaption(QObject::tr("Weather Balloon"));
    app.setMainWidget(&balloon);
    QObject::connect(&balloon, SIGNAL(clicked()),
                     &app, SLOT(quit()));
    balloon.show();
    return app.exec();
}
