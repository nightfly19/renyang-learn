#include "checklists.h"
#include <qapplication.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    ListView *piv = new ListView();
    app.setMainWidget(piv);
    piv->show();

    return app.exec();
}
