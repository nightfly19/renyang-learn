#include <qapplication.h>

#include "threadform.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ThreadForm form;
    app.setMainWidget(&form);
    form.show();
    return app.exec();
}
