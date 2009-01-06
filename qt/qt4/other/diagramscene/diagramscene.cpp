
#include <QtGui>

#include "diagramscene.h"
#include "arrow.h"

//! [0]
DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent)	// itemMenu是表示對物件按右鍵會跳出來的menu,目前的menu只有在按item時有效
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;	// 記錄目前的item行態
    myMode = MoveItem;		// 一開始是MoveItem,表示可以移動物件
    myItemType = DiagramItem::Step;
    line = 0;
    textItem = 0;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
}
//! [0]

//! [1]
void DiagramScene::setLineColor(const QColor &color)
{
    myLineColor = color;
    if (isItemChange(Arrow::Type)) {
        Arrow *item =
            qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}
//! [1]

//! [2]
void DiagramScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}
//! [2]

//! [3]
void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item =
            qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        item->setBrush(myItemColor);
    }
}
//! [3]

//! [4]
void DiagramScene::setFont(const QFont &font)
{
    myFont = font;	// 設定scene目前的字型設定

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setFont(myFont);
    }
}
//! [4]

void DiagramScene::setMode(Mode mode)
{
    myMode = mode;	// 設定目前scene的模式:一共有四個模式啊:InsertItem, InsertLine, InsertText, MoveItem
}

void DiagramScene::setItemType(DiagramItem::DiagramType type)
{
    myItemType = type;	// scene item的模式:Step, Conditional, StartEnd, Io
}

//! [5]
void DiagramScene::editorLostFocus(DiagramTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }
}
//! [5]

//! [6]
void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)	// 不知道為什麼按下物件時,那個物件會被選取
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    DiagramItem *item;
    switch (myMode) {	// 判斷scene的mode:InsertItem, InsertLine, InsertText, MoveItem
        case InsertItem:			// 若是insert mode則產生一個物件在scene上
            item = new DiagramItem(myItemType, myItemMenu);	// 依myItemType與myItemMenu來產生要放到scene上的物件
            item->setBrush(myItemColor);			// 設定顏色
            addItem(item);					// 把目前的item加到scene中
            item->setPos(mouseEvent->scenePos());		// 依滑鼠的位置放置一個選的物件
            emit itemInserted(item);		// 只有當是itemMode把物件放到scene之後,才會設定按扭跳回來
            break;
//! [6] //! [7]
        case InsertLine:			// 加入line到scene中
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),mouseEvent->scenePos())); // 設定一條線,但是起點與終點是同一個點
            line->setPen(QPen(myLineColor, 2));		// 設定顏色,與粗細
            addItem(line);				// 加入scene中
            break;
//! [7] //! [8]
        case InsertText:			// 加入text
            textItem = new DiagramTextItem();	// 產生DiagramTextItem()
            textItem->setFont(myFont);		// 設定字型
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setZValue(1000.0);
            connect(textItem, SIGNAL(lostFocus(DiagramTextItem *)),
                    this, SLOT(editorLostFocus(DiagramTextItem *)));
            connect(textItem, SIGNAL(selectedChange(QGraphicsItem *)),
                    this, SIGNAL(itemSelected(QGraphicsItem *)));
            addItem(textItem);				// 把text物件加入scene中
            textItem->setDefaultTextColor(myTextColor);	// 設定顏色
            textItem->setPos(mouseEvent->scenePos());	// 設定加入的位置
            emit textInserted(textItem);	// 當是文字按扭時,當按到scene之後,按扭也會跳回來
	    break;
//! [8] //! [9]
    default:
    }
    QGraphicsScene::mousePressEvent(mouseEvent);	// 當按下滑鼠時,此物件會被選取
}
//! [9]

//! [10]
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == InsertLine && line != 0) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);	// 當按下滑鼠並且移動指標時,此物件位置會被移動
    }
}
//! [10]

//! [11]
void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (line != 0 && myMode == InsertLine) {	// 當畫面上有一條線,且目前的scene的mode是InsertLine
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        removeItem(line);
        delete line;
//! [11] //! [12]

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first()->type() == DiagramItem::Type &&
            endItems.first()->type() == DiagramItem::Type &&
            startItems.first() != endItems.first()) {
            DiagramItem *startItem =
                qgraphicsitem_cast<DiagramItem *>(startItems.first());
            DiagramItem *endItem =
                qgraphicsitem_cast<DiagramItem *>(endItems.first());
            Arrow *arrow = new Arrow(startItem, endItem);
            arrow->setColor(myLineColor);
            startItem->addArrow(arrow);
            endItem->addArrow(arrow);
            arrow->setZValue(-1000.0);
            addItem(arrow);
            arrow->updatePosition();
        }
    }
//! [12] //! [13]
    line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
//! [13]

//! [14]
bool DiagramScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}
//! [14]
