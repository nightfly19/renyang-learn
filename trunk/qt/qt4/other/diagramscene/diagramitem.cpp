
#include <QtGui>

#include "diagramitem.h"
#include "arrow.h"

//! [0]
DiagramItem::DiagramItem(DiagramType diagramType, QMenu *contextMenu,
             QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsPolygonItem(parent, scene)
{
    myDiagramType = diagramType;
    myContextMenu = contextMenu;

    QPainterPath path;
    switch (myDiagramType) {
        case StartEnd:
            path.moveTo(200, 50);
            path.arcTo(150, 0, 50, 50, 0, 90);
            path.arcTo(50, 0, 50, 50, 90, 90);
            path.arcTo(50, 50, 50, 50, 180, 90);
            path.arcTo(150, 50, 50, 50, 270, 90);
            path.lineTo(200, 25);
            myPolygon = path.toFillPolygon();
            break;
        case Conditional:
            myPolygon << QPointF(-100, 0) << QPointF(0, 100)
                      << QPointF(100, 0) << QPointF(0, -100)
                      << QPointF(-100, 0);
            break;
        case Step:
            myPolygon << QPointF(-100, -100) << QPointF(100, -100)
                      << QPointF(100, 100) << QPointF(-100, 100)
                      << QPointF(-100, -100);
            break;
        default:
            myPolygon << QPointF(-120, -80) << QPointF(-70, 80)
                      << QPointF(120, 80) << QPointF(70, -80)
                      << QPointF(-120, -80);
            break;
    }
    setPolygon(myPolygon);		// 先設定好圖行的形狀,再把這一個圖形設定給本類別
    setFlag(QGraphicsItem::ItemIsMovable, true);	// 設定使得物件可以移動,找了一個下午總算找到了為什麼此物件可以被選擇了
    setFlag(QGraphicsItem::ItemIsSelectable, true);	// 設定使得物件可以被選擇
}
//! [0]

//! [1]
void DiagramItem::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);

    if (index != -1)
        arrows.removeAt(index);
}
//! [1]

//! [2]
void DiagramItem::removeArrows()
{
    foreach (Arrow *arrow, arrows) {
        arrow->startItem()->removeArrow(arrow);
        arrow->endItem()->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}
//! [2]

//! [3]
void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}
//! [3]

//! [4]
QPixmap DiagramItem::image() const	// 要讓ToolButton設定的小圖像
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));		// 設定QToolButton小圖像的顏色
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);

    return pixmap;
}
//! [4]

//! [5]
void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)	// 當滑鼠在此物件內部按下右鍵時,會跳出QMenu
{
    scene()->clearSelection();	// 把所有此scene被選取的items取消選取
    setSelected(true);		// 設定目前這一個item是被選取的
    myContextMenu->exec(event->screenPos());	// 在這個位置跳出itmeMenu
}
//! [5]

//! [6]
QVariant DiagramItem::itemChange(GraphicsItemChange change,	// 當item改變時，此function會被觸發,本來是virtual function
                     const QVariant &value)
// change is the parameter of the item that is changing. value is the new value; the type of the value depends on change.
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Arrow *arrow, arrows) {
            arrow->updatePosition();	// 更新箭頭的新座標
        }
    }

    return value;
}
//! [6]
