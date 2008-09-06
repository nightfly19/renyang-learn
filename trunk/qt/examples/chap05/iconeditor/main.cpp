#include <qapplication.h>

#include "iconeditor.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    IconEditor iconEditor;
    iconEditor.setCaption(QObject::tr("Icon Editor"));
    iconEditor.setIconImage(QImage::fromMimeSource("mouse.png"));
    app.setMainWidget(&iconEditor);
    iconEditor.show();
    return app.exec();
}
