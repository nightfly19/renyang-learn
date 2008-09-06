#include <qapplication.h>

#include "tripplanner.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TripPlanner tripPlanner;
    app.setMainWidget(&tripPlanner);
    tripPlanner.show();
    return app.exec();
}
