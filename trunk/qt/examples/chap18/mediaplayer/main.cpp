#include <qapplication.h>

#include "playerwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PlayerWindow playerWin;
    app.setMainWidget(&playerWin);
    playerWin.show();
    return app.exec();
}
