
#ifndef MYVIEW_H
#define MYVIEW_H

#include "mydebug.h"

#include "myqcanvas.h"

class Caterpillar;

class View : public QCanvasView {
Q_OBJECT
public:
    View(myQCanvas& canvas);
public slots:
    void control_center();
private:
    Caterpillar *cater2;
};

#endif //MYVIEW_H

