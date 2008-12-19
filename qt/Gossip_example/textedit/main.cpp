#include "simreader.h"
#include <qapplication.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    SimReader reader;
    app.setMainWidget(&reader);

    reader.show();

    return app.exec();
}
