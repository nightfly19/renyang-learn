#include <qapplication.h>

#include "weatherstation.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    WeatherStation station;
    app.setMainWidget(&station);
    station.show();
    return app.exec();
}
