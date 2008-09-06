#include <qapplication.h>
#include <qpushbutton.h>

#include "tripserver.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TripServer server;
    if (!server.ok()) {
        qWarning("Failed to bind to port");
        return 1;
    }
    QPushButton quitButton(QObject::tr("&Quit"), 0);
    quitButton.setCaption(QObject::tr("Trip Server"));
    app.setMainWidget(&quitButton);
    QObject::connect(&quitButton, SIGNAL(clicked()),
                     &app, SLOT(quit()));
    quitButton.show();
    return app.exec();
}
