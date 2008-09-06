#include <qaction.h>
#include <qapplication.h>
#include <qclipboard.h>

#include "mytable.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MyTable table;
    table.setCaption(QObject::tr("Cell Drag"));
    app.setMainWidget(&table);

    QAction *copyAct = new QAction(&table);
    copyAct->setAccel(QObject::tr("Ctrl+C"));
    QObject::connect(copyAct, SIGNAL(activated()),
                     &table, SLOT(copy()));

    QAction *pasteAct = new QAction(&table);
    pasteAct->setAccel(QObject::tr("Ctrl+V"));
    QObject::connect(pasteAct, SIGNAL(activated()),
                     &table, SLOT(paste()));

    table.show();
    return app.exec();
}
