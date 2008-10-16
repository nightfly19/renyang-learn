
#include <qapplication.h>
#include "myview.h"
#include "caterpillar.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    myQCanvas canvas(0, 0);

    canvas.setAdvancePeriod(500);  // 移動更新間隔:
    canvas.setDoubleBuffering(true); // double buffer
    View c(canvas);

    app.setMainWidget(&c);
    c.show();

    return app.exec();
} 
