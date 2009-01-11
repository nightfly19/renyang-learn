#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QListWidget>

class QMouseEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;
class QPoint;

class ListWidget : public QListWidget {
    Q_OBJECT
    
public:
    ListWidget(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    
private:
    void execDrag();
    QPoint startPoint;
};

#endif
