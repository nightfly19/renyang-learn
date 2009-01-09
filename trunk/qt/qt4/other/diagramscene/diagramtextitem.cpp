
#include <QtGui>

#include "diagramtextitem.h"
#include "diagramscene.h"

//! [0]
DiagramTextItem::DiagramTextItem(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsTextItem(parent, scene)
{
    setFlag(QGraphicsItem::ItemIsMovable);	// 設定可以被移動
    setFlag(QGraphicsItem::ItemIsSelectable);	// 設定可以被選擇
}
//! [0]

//! [1]
QVariant DiagramTextItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)	// 在父類別中是virtual function
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        emit selectedChange(this);
    return value;
}
//! [1]

//! [2]
void DiagramTextItem::focusOutEvent(QFocusEvent *event)	// 當由選取,變到沒有被選取,則會自動執行這一個函式
{
    setTextInteractionFlags(Qt::NoTextInteraction);	// 設定文字輸入的方式
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}
//! [2]

//! [5]
void DiagramTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction)
        setTextInteractionFlags(Qt::TextEditorInteraction);	// 改成可以被修改內容
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}
//! [5]
