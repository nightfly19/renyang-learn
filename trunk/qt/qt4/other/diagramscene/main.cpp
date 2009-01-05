
#include <QtGui>

#include "mainwindow.h"

int main(int argv, char *args[])
{
    Q_INIT_RESOURCE(diagramscene);

    QApplication app(argv, args);
    MainWindow mainWindow;
    mainWindow.setGeometry(100, 100, 800, 500);	// 設定此視窗位置在(100,100),其大小為800寬;500高
    mainWindow.show();

    return app.exec();
}
