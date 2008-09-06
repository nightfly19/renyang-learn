#include <qapplication.h>

#include "finddialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FindDialog *dialog = new FindDialog;
    app.setMainWidget(dialog);
    dialog->show();
    return app.exec();
}
