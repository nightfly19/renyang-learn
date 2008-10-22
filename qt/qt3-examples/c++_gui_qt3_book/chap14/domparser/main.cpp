#include <qapplication.h>
#include <qfile.h>
#include <qlistview.h>

#include "domparser.h"

void parseFile(const QString &fileName)
{
    QListView *listView = new QListView(0);
    listView->setCaption(QObject::tr("DOM Parser"));
    listView->setRootIsDecorated(true);
    listView->setResizeMode(QListView::AllColumns);
    listView->addColumn(QObject::tr("Terms"));
    listView->addColumn(QObject::tr("Pages"));
    listView->show();

    QFile file(fileName);
    DomParser(&file, listView);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (argc < 2) {
        qWarning("Usage: domparser file1.xml...");
        return 1;
    }

    for (int i = 1; i < argc; ++i)
        parseFile(argv[i]);

    QObject::connect(&app, SIGNAL(lastWindowClosed()),
                     &app, SLOT(quit()));
    return app.exec();
}
