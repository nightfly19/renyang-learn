#include <QApplication>
 
#include "scftp.h"
 
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ScFtp *dialog = new ScFtp;
 
    dialog->show();
    return app.exec();
}
