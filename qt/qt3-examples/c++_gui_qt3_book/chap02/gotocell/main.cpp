#include <qapplication.h>

#include "gotocelldialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    GoToCellDialog *dialog = new GoToCellDialog;
    app.setMainWidget(dialog);
    dialog->show();
    return app.exec();
}
