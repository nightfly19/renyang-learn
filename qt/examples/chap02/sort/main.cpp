#include <qapplication.h>

#include "sortdialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SortDialog *dialog = new SortDialog;
    app.setMainWidget(dialog);
    dialog->setColumnRange('C', 'F');
    dialog->show();
    return app.exec();
}
