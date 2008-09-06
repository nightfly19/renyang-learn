#include <qapplication.h>

#include "imagewindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ImageWindow imageWin;
    app.setMainWidget(&imageWin);
    imageWin.resize(400, 300);
    imageWin.show();
    return app.exec();
}
