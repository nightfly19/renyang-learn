#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H

#include <qcanvas.h>

class QAction;

class DiagramView : public QCanvasView
{
    Q_OBJECT
public:
    DiagramView(QCanvas *canvas, QWidget *parent = 0,
                const char *name = 0);

public slots:
    void cut();
    void copy();
    void paste();
    void del();
    void properties();
    void addBox();
    void addLine();
    void bringToFront();
    void sendToBack();

protected:
    void contentsContextMenuEvent(QContextMenuEvent *event);
    void contentsMousePressEvent(QMouseEvent *event);
    void contentsMouseMoveEvent(QMouseEvent *event);
    void contentsMouseDoubleClickEvent(QMouseEvent *event);

private:
    void createActions();
    void addItem(QCanvasItem *item);
    void setActiveItem(QCanvasItem *item);
    void showNewItem(QCanvasItem *item);

    QCanvasItem *pendingItem;
    QCanvasItem *activeItem;
    QPoint lastPos;
    int minZ;
    int maxZ;

    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *deleteAct;
    QAction *propertiesAct;
    QAction *addBoxAct;
    QAction *addLineAct;
    QAction *bringToFrontAct;
    QAction *sendToBackAct;
};

class DiagramBox : public QCanvasRectangle
{
public:
    enum { RTTI = 1001 };

    DiagramBox(QCanvas *canvas);
    ~DiagramBox();

    void setText(const QString &newText);
    QString text() const { return str; }
    void drawShape(QPainter &painter);
    QRect boundingRect() const;
    int rtti() const { return RTTI; }

private:
    QString str;
};

class DiagramLine : public QCanvasLine
{
public:
    enum { RTTI = 1002 };

    DiagramLine(QCanvas *canvas);
    ~DiagramLine();

    QPoint offset() const { return QPoint((int)x(), (int)y()); }
    void drawShape(QPainter &painter);
    QPointArray areaPoints() const;
    int rtti() const { return RTTI; }
};

#endif
