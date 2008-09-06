#include <qapplication.h>

#include "diagramview.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCanvas canvas;
    canvas.resize(640, 480);
    DiagramView view(&canvas);
    view.setCaption(QObject::tr("Diagram"));
    app.setMainWidget(&view);
    view.show();
    return app.exec();
}
