#include <qapplication.h>
#include <qsqldatabase.h>

#include "connection.h"
#include "mainform.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    if (!createConnections())
        return 1;

    MainForm mainForm;
    app.setMainWidget(&mainForm);
    mainForm.resize(480, 320);
    mainForm.show();
    return app.exec();
}
