#include <qapplication.h>

#include "convertdialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ConvertDialog dialog;
    app.setMainWidget(&dialog);
    dialog.show();
    return app.exec();
}
