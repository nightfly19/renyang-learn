#ifndef __FIGURE_EDITOR_H__
#define __FIGURE_EDITOR_H__

#include <qpopupmenu.h>
#include <qmainwindow.h>
#include <qintdict.h>
#include <qcanvas.h>

class Car;
class FigureEditor : public QCanvasView {
    Q_OBJECT

public:
    FigureEditor(QCanvas&, QWidget* parent=0, const char* name=0, WFlags f=0);
    void clear();

protected:
    void contentsMousePressEvent(QMouseEvent*);
    void contentsMouseMoveEvent(QMouseEvent*);
    void contentsMouseReleaseEvent(QMouseEvent*);

signals:
    void status(const QString&);
    void leftSelect(QCanvasItem*); //Car only now
    void rightSelect(QCanvasItem*); 
    void sigMouseRelease(); 
    void sigSelACar(Car*);

private:
    QCanvasItem* moving;
    QPoint moving_start;
};

#endif
