#include "dclock.h"
#include <qapplication.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    DigitalClock *clock = new DigitalClock;

    clock->resize(170, 80);
    clock->setCaption("Digital Clock");

    app.setMainWidget(clock);

    clock->show();

    return app.exec();
}
