
#include <QtGui>

#include "mainwindow.h"

int main(int argv, char *args[])
{
    Q_INIT_RESOURCE(diagramscene);		// 把載入的資料存到diagramscene中

    QApplication app(argv, args);
    MainWindow mainWindow;
    mainWindow.setGeometry(100, 100, 800, 500);	// 設定此視窗位置在(100,100),其大小為800寬;500高
    mainWindow.show();				// 設定顯示

    return app.exec();
}
