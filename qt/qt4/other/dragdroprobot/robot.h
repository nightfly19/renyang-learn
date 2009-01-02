
#ifndef ROBOT_H
#define ROBOT_H

#include <QGraphicsItem>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QTimeLine;
QT_END_NAMESPACE

class RobotPart : public QGraphicsItem
{
public:
    RobotPart(QGraphicsItem *parent = 0);

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);

    QPixmap pixmap;
    QColor color;
    bool dragOver;
};

class RobotHead : public RobotPart
{
public:
    RobotHead(QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    enum { Type = UserType + 1 };	// 不知道UserType是哪裡來的
    int type() const;
};

class RobotTorso : public RobotPart
{
public:
    RobotTorso(QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
};

class RobotLimb : public RobotPart
{
public:
    RobotLimb(QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
};

class Robot : public RobotPart
{
public:
    Robot();
    ~Robot();
    
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    QTimeLine *timeLine;	// 主要是建立這一個timeLine
};

#endif
