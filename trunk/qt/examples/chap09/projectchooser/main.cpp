#include <qapplication.h>

#include "projectchooser.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ProjectChooser projectChooser;
    app.setMainWidget(&projectChooser);
    projectChooser.show();
    return app.exec();
}
