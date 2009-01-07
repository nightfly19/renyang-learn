
#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsLineItem>

#include "diagramitem.h"

QT_BEGIN_NAMESPACE
class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;
QT_END_NAMESPACE

//! [0]
// 這一個類別是用來建立箭頭
class Arrow : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4 };	// 設定一個匿名enum

    Arrow(DiagramItem *startItem, DiagramItem *endItem,		// 設定起始物件與結束物件,用來決定箭頭的起點與終點
      QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    int type() const
        { return Type; }					// 回傳type用來辨識是哪一種GraphicsItem
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setColor(const QColor &color)
        { myColor = color; }
    DiagramItem *startItem() const
        { return myStartItem; }
    DiagramItem *endItem() const
        { return myEndItem; }


public slots:
    void updatePosition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

private:
    DiagramItem *myStartItem;
    DiagramItem *myEndItem;
    QColor myColor;
    QPolygonF arrowHead;
};
//! [0]

#endif
