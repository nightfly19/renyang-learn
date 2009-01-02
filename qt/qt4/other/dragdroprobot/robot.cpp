
#include <QtGui>

#include "robot.h"

RobotPart::RobotPart(QGraphicsItem *parent)
    : QGraphicsItem(parent), color(Qt::lightGray), dragOver(false)
{
    setAcceptDrops(true);
}

void RobotPart::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasColor()	// 可以回傳QColor
        || (qgraphicsitem_cast<RobotHead *>(this) && event->mimeData()->hasImage())) {
	// 把this試著轉換成RobotHead的指標,若失敗則傳回0
        event->setAccepted(true);
        dragOver = true;
        update();
    } else {
        event->setAccepted(false);	// 接受拖曳
    }
}

void RobotPart::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);	// 告知編譯器沒有使用到event這一個變數,否則編譯器會產生warning
    dragOver = false;
    update();
}

void RobotPart::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    dragOver = false;
    if (event->mimeData()->hasColor())
        color = qVariantValue<QColor>(event->mimeData()->colorData());	// 因為colorData()回傳的是QVariant,要用qVariantValue轉換成QColor
    else if (event->mimeData()->hasImage())
        pixmap = qVariantValue<QPixmap>(event->mimeData()->imageData()); // 同上
    update();
}

RobotHead::RobotHead(QGraphicsItem *parent) : RobotPart(parent)
{
	// do nothing
}

QRectF RobotHead::boundingRect() const
{
    return QRectF(-15, -50, 30, 50);	// 設定原點在小座標系統的(-15,-50)
}

void RobotHead::paint(QPainter *painter,	// 重新畫頭
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);	// 設定用不到
    Q_UNUSED(widget);	// 設定用不到
    if (pixmap.isNull()) {	// 若pixmap是null的話,則表示是QColor啦
        painter->setBrush(dragOver ? color.light(130) : color);
        painter->drawRoundedRect(-10, -30, 20, 30, 25, 25, Qt::RelativeSize); // 建立一個圓角的四角形,是依%來設定四個角的彎度
        painter->setBrush(Qt::white);
        painter->drawEllipse(-7, -3 - 20, 7, 7);
        painter->drawEllipse(0, -3 - 20, 7, 7);
        painter->setBrush(Qt::black);
        painter->drawEllipse(-5, -1 - 20, 2, 2);
        painter->drawEllipse(2, -1 - 20, 2, 2);
        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawArc(-6, -2 - 20, 12, 15, 190 * 16, 160 * 16);
    } else {
        painter->scale(.2272, .2824);	// 若是圖片則縮小
        painter->drawPixmap(QPointF(-15 * 4.4, -50 * 3.54), pixmap);
    }
}

int RobotHead::type() const
{
    return Type;
}

RobotTorso::RobotTorso(QGraphicsItem *parent)	// Robot的身體
    : RobotPart(parent)
{
	// do nothing
}

QRectF RobotTorso::boundingRect() const
{
    return QRectF(-30, -20, 60, 60);
}

void RobotTorso::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setBrush(dragOver ? color.light(130) : color);
    painter->drawRoundedRect(-20, -20, 40, 60, 25, 25, Qt::RelativeSize);
    painter->drawEllipse(-25, -20, 20, 20);
    painter->drawEllipse(5, -20, 20, 20);
    painter->drawEllipse(-20, 22, 20, 20);
    painter->drawEllipse(0, 22, 20, 20);
}

RobotLimb::RobotLimb(QGraphicsItem *parent)	// Robot的四肢
    : RobotPart(parent)
{
	// do nothing
}

QRectF RobotLimb::boundingRect() const
{
    return QRectF(-5, -5, 40, 10);
}

void RobotLimb::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(dragOver ? color.light(130) : color);
    painter->drawRoundedRect(boundingRect(), 50, 50, Qt::RelativeSize);
    painter->drawEllipse(-5, -5, 10, 10);
}

Robot::Robot()	// 整個Robot
{
    QGraphicsItem *torsoItem = new RobotTorso(this);    
    QGraphicsItem *headItem = new RobotHead(torsoItem);
    QGraphicsItem *upperLeftArmItem = new RobotLimb(torsoItem);
    QGraphicsItem *lowerLeftArmItem = new RobotLimb(upperLeftArmItem);
    QGraphicsItem *upperRightArmItem = new RobotLimb(torsoItem);
    QGraphicsItem *lowerRightArmItem = new RobotLimb(upperRightArmItem);
    QGraphicsItem *upperRightLegItem = new RobotLimb(torsoItem);
    QGraphicsItem *lowerRightLegItem = new RobotLimb(upperRightLegItem);
    QGraphicsItem *upperLeftLegItem = new RobotLimb(torsoItem);
    QGraphicsItem *lowerLeftLegItem = new RobotLimb(upperLeftLegItem);
    
    headItem->setPos(0, -18);	// 設定頭的原點位置
    upperLeftArmItem->setPos(-15, -10);	// 左臂
    lowerLeftArmItem->setPos(30, 0);	// 左腳
    upperRightArmItem->setPos(15, -10);	// 右臂
    lowerRightArmItem->setPos(30, 0);	// 右腳
    upperRightLegItem->setPos(10, 32);	// 右手挽
    lowerRightLegItem->setPos(30, 0);	// 右腳挽
    upperLeftLegItem->setPos(-10, 32);	// 左手挽
    lowerLeftLegItem->setPos(30, 0);	// 左腳挽

    timeLine = new QTimeLine;	// 設定整個時間的timerline

    QGraphicsItemAnimation *headAnimation = new QGraphicsItemAnimation;	// 設定頭部的動畫
    headAnimation->setItem(headItem);		// 把頭加入動畫的配製
    headAnimation->setTimeLine(timeLine);	// 把動畫設定timerline
    headAnimation->setRotationAt(0, 20);	// 設定在step=0其rotate是20
    headAnimation->setRotationAt(1, -20);	// 設定在step=1其rotate是-20
    headAnimation->setScaleAt(1, 11, 11);	// 若沒有設定 step=0是多少,則表是沒有變化,而當step=1時,會放大11倍

    QGraphicsItemAnimation *upperLeftArmAnimation = new QGraphicsItemAnimation;
    upperLeftArmAnimation->setItem(upperLeftArmItem);
    upperLeftArmAnimation->setTimeLine(timeLine);	// 加入timerline
    upperLeftArmAnimation->setRotationAt(0, 190);	// 當step=0,rotate是190
    upperLeftArmAnimation->setRotationAt(1, 180);	// 當step=1,rotate是180

    QGraphicsItemAnimation *lowerLeftArmAnimation = new QGraphicsItemAnimation;
    lowerLeftArmAnimation->setItem(lowerLeftArmItem);
    lowerLeftArmAnimation->setTimeLine(timeLine);
    lowerLeftArmAnimation->setRotationAt(0, 50);
    lowerLeftArmAnimation->setRotationAt(1, 10);
    
    QGraphicsItemAnimation *upperRightArmAnimation = new QGraphicsItemAnimation;
    upperRightArmAnimation->setItem(upperRightArmItem);
    upperRightArmAnimation->setTimeLine(timeLine);
    upperRightArmAnimation->setRotationAt(0, 300);
    upperRightArmAnimation->setRotationAt(1, 310);

    QGraphicsItemAnimation *lowerRightArmAnimation = new QGraphicsItemAnimation;
    lowerRightArmAnimation->setItem(lowerRightArmItem);
    lowerRightArmAnimation->setTimeLine(timeLine);
    lowerRightArmAnimation->setRotationAt(0, 0);
    lowerRightArmAnimation->setRotationAt(1, -70);

    QGraphicsItemAnimation *upperLeftLegAnimation = new QGraphicsItemAnimation;
    upperLeftLegAnimation->setItem(upperLeftLegItem);
    upperLeftLegAnimation->setTimeLine(timeLine);
    upperLeftLegAnimation->setRotationAt(0, 150);
    upperLeftLegAnimation->setRotationAt(1, 80);

    QGraphicsItemAnimation *lowerLeftLegAnimation = new QGraphicsItemAnimation;
    lowerLeftLegAnimation->setItem(lowerLeftLegItem);
    lowerLeftLegAnimation->setTimeLine(timeLine);
    lowerLeftLegAnimation->setRotationAt(0, 70);
    lowerLeftLegAnimation->setRotationAt(1, 10);

    QGraphicsItemAnimation *upperRightLegAnimation = new QGraphicsItemAnimation;
    upperRightLegAnimation->setItem(upperRightLegItem);
    upperRightLegAnimation->setTimeLine(timeLine);
    upperRightLegAnimation->setRotationAt(0, 40);
    upperRightLegAnimation->setRotationAt(1, 120);
    
    QGraphicsItemAnimation *lowerRightLegAnimation = new QGraphicsItemAnimation;
    lowerRightLegAnimation->setItem(lowerRightLegItem);
    lowerRightLegAnimation->setTimeLine(timeLine);
    lowerRightLegAnimation->setRotationAt(0, 10);
    lowerRightLegAnimation->setRotationAt(1, 50);
    
    QGraphicsItemAnimation *torsoAnimation = new QGraphicsItemAnimation;
    torsoAnimation->setItem(torsoItem);
    torsoAnimation->setTimeLine(timeLine);
    torsoAnimation->setRotationAt(0, 5);
    torsoAnimation->setRotationAt(1, -20);

    timeLine->setUpdateInterval(1000 / 25);	// 設定每一次更新畫面是1000ms/25
    timeLine->setCurveShape(QTimeLine::SineCurve);	// 用時間來決定step的值step的值是由 -1到1之間
    timeLine->setLoopCount(0);	// 設定迴圈次數,0表示無線迴圈
    timeLine->setDuration(2000); // 設定每一次迴圈是設定為2000ms
    timeLine->start();	// 開始啦
}

Robot::~Robot()
{
    delete timeLine;
}

QRectF Robot::boundingRect() const
{
    return QRectF();
}

void Robot::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}
