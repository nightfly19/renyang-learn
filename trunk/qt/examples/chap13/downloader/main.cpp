#include <qapplication.h>

#include "downloader.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QUrl url("ftp://ftp.example.com/");
    if (argc >= 2)
        url = argv[1];
    Downloader downloader(url);
    QObject::connect(&downloader, SIGNAL(finished()),
                     &app, SLOT(quit()));
    return app.exec();
}
