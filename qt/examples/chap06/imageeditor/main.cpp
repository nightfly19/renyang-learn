#include <qapplication.h>

#include "imageeditor.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ImageEditor imageEditor;
    imageEditor.setCaption(QObject::tr("Image Editor"));
    imageEditor.setImage(QImage::fromMimeSource("mouse.png"));
    app.setMainWidget(&imageEditor);
    imageEditor.show();
    return app.exec();
}
