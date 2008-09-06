#include <qapplication.h>

#include "hexspinbox.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    HexSpinBox spinBox(0);
    spinBox.setCaption(QObject::tr("Hex Spin Box"));
    app.setMainWidget(&spinBox);
    spinBox.show();
    return app.exec();
}
