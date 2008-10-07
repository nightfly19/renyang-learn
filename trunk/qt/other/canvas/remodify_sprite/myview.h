
#ifndef MYVIEW_H
#define MYVIEW_H

#include "mydebug.h"

#include <qcanvas.h>

class View : public QCanvasView {
public:
    View(QCanvas& canvas);
};

#endif //MYVIEW_H

