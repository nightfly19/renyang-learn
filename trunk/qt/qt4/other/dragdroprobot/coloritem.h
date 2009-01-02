
#ifndef COLORITEM_H
#define COLORITEM_H

#include <QGraphicsItem>

// 設定有顏色的球
class ColorItem : public QGraphicsItem
{
public:
    ColorItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QColor color;
};

#endif
