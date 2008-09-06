#include <qapplication.h>
#include <qvbox.h>
#include <qlayout.h>

#include "ticker.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Ticker ticker;
    ticker.setCaption(QObject::tr("Ticker"));
    app.setMainWidget(&ticker);
    ticker.setText(QObject::tr("How long it lasted was impossible "
                               "to say ++ "));
    ticker.show();
    return app.exec();
}
