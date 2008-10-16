
#ifndef MYVIEW_H
#define MYVIEW_H

#include "mydebug.h"

#include "myqcanvas.h"

class View : public QCanvasView {
public:
    View(myQCanvas& canvas);
};

#endif //MYVIEW_H

