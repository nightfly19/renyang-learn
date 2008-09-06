#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include <qlistbox.h>

class ProjectView : public QListBox
{
    Q_OBJECT
public:
    ProjectView(QWidget *parent, const char *name = 0);

protected:
    void contentsMousePressEvent(QMouseEvent *event);
    void contentsMouseMoveEvent(QMouseEvent *event);
    void contentsDragEnterEvent(QDragEnterEvent *event);
    void contentsDropEvent(QDropEvent *event);

private:
    void startDrag();

    QPoint dragPos;
};

#endif
