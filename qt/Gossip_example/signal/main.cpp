/*
 * Name:
 * Copyright:
 * Author:
 * Date: 16/12/08 22:32
 * Description: 修改為每按十次會顯示累加的次數，而不會歸零
 */

#include <qapplication.h>
#include "showpush.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    ShowPush wm;
    wm.setGeometry(100, 100, 250, 150);
    app.setMainWidget(&wm);
    wm.show();

    return app.exec();
}
