
#include <QtGui>

#include "arrow.h"
#include <math.h>

const qreal Pi = 3.14;

//! [0]
Arrow::Arrow(DiagramItem *startItem, DiagramItem *endItem,
         QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsLineItem(parent, scene)	// QGraphicsLineItem::QGraphicsLineItem(QGraphicsItem* parent, QGraphicsScene* scene)
{
    myStartItem = startItem;
    myEndItem = endItem;
    setFlag(QGraphicsItem::ItemIsSelectable, true);	// 設定可以直接被滑鼠選擇
    myColor = Qt::black;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}
//! [0]

//! [1]
QRectF Arrow::boundingRect() const	// 設定要重畫的區域,使得QGraphicsView知道哪一個區域要重畫
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),line().p2().y() - line().p1().y()))	// 設定起始點,長寬
        .normalized()	// 若width()<0則左上點與右下點與對應的點左右互換;若height()<0則左上點與右下點與對應的上下點互換
        .adjusted(-extra, -extra, extra, extra);	// 左上點座標加(-extra,-extra),右下點座標加(extra,extra)
	// adjusted(x1,y1,x2,y2)是設定rect的左上角的點是(x1,y1)和右下角的點是(x2,y2)
	// 回傳修改完之後的方形
}
//! [1]

//! [2]
QPainterPath Arrow::shape() const
{
    // virtual預設是呼叫boundingRect(),但是,我們現要實做這一個function
    // 此function是給collision detection, hit tests, QGraphicsScene::items()使用
    QPainterPath path = QGraphicsLineItem::shape();	// 回傳線的path
    path.addPolygon(arrowHead);	// 在線上面加一個箭頭
    return path;
}
//! [2]

//! [3]
void Arrow::updatePosition()	// 更新箭頭的起點與終點
{
    // Maps the myStartItem point, which is in item's coordinate system, to this item's coordinate system, and returns the mapped coordinate.
    // 把物件中的(0,0)座標轉換成scene的座標,並且設定為線的開始和結束
    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);
}
//! [3]

//! [4]
void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)	// 畫出箭頭,每移動畫部的東西,就會執行paint
{
    if (myStartItem->collidesWithItem(myEndItem))	// 若起始物件與終點物件有發生碰撞,則不要畫圖啦
        return;

    QPen myPen = pen();		// 把目前的QGraphicsLine的pen參數取出來,Returns the item's pen, or a black solid 0-width pen if no pen has been set
    myPen.setColor(myColor);	// 設定顏色
    qreal arrowSize = 20;	// 設定箭頭前面的大小
    painter->setPen(myPen);	// 設定外圍的顏色
    painter->setBrush(myColor);	// 設定箭頭內部的顏色
//! [4] //! [5]

    // myStartItem->pos():表示回傳(0,0)在myStartItem內的點,轉換到parent的coordinate(),若沒有父類別,則轉換到scene的座標
    QLineF centerLine(myStartItem->pos(), myEndItem->pos());	// 以myStartItem與myEndItem(0,0)為起點與終點畫線(不是箭頭)
    QPolygonF endPolygon = myEndItem->polygon();		// 回傳myEndItem的所有相關的座標點
    QPointF p1 = endPolygon.first() + myEndItem->pos();		// first()的部分會回傳item自己的座標系統的左上角的座標
    QPointF p2;
    QPointF intersectPoint;
    QLineF polyLine;
    for (int i = 1; i < endPolygon.count(); ++i) {
    p2 = endPolygon.at(i) + myEndItem->pos();	// 相加表示轉換的意思,由item的座標轉換成scene的座標,因為pos()表示在(0,0)轉換到parent的座標
    polyLine = QLineF(p1, p2);
    QLineF::IntersectType intersectType =
        polyLine.intersect(centerLine, &intersectPoint);	// 此兩條線是否有交集,intersectPoint會儲存那一個交集的點
    if (intersectType == QLineF::BoundedIntersection)		// 若是延長線才有交集,則表示找到了,跳離迴圈
        break;
        p1 = p2;
    }

    setLine(QLineF(intersectPoint, myStartItem->pos()));
//! [5] //! [6]

    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)	// 若第一個點減掉第二個點是負的,表示其夾角大於90度(由左上到右下的箭頭)
        angle = (Pi * 2) - angle;

        QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,	// 計算出箭頭的三角型的左端
                                        cos(angle + Pi / 3) * arrowSize);
        QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,	// 計算出箭頭的三角型的右端
                                        cos(angle + Pi - Pi / 3) * arrowSize);

        arrowHead.clear();	// 把線清空
        arrowHead << line().p1() << arrowP1 << arrowP2;	// 加入三角型
//! [6] //! [7]
        painter->drawLine(line());	// 畫線
        painter->drawPolygon(arrowHead);// 畫三角型
        if (isSelected()) {
            painter->setPen(QPen(myColor, 1, Qt::DashLine));	// 設定當此箭頭被選取時,要用什麼線條把它框起來
        QLineF myLine = line();					// 回傳此物件線(主線,不包含前面的箭頭)
        myLine.translate(0, 4.0);				// 設定所有點的y值加4
        painter->drawLine(myLine);				// 畫出上面包住此線的框
        myLine.translate(0,-8.0);				// 把所有的點的y值-8(若是減4則回到原來的位置)
        painter->drawLine(myLine);				// 畫出下面包住此線的框
    }
}
//! [7]
