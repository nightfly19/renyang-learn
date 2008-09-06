#include <qapplication.h>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWin;
    app.setMainWidget(&mainWin);

    if (argc > 1) {
        for (int i = 1; i < argc; ++i)
            mainWin.openFile(argv[i]);
    } else {
        mainWin.newFile();
    }

    mainWin.show();
    return app.exec();
}
