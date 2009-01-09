
#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QGraphicsScene>
#include "diagramitem.h"
#include "diagramtextitem.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
QT_END_NAMESPACE

//! [0]
class DiagramScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertItem, InsertLine, InsertText, MoveItem };	// 設定目前scene一共有四種模式

    DiagramScene(QMenu *itemMenu, QObject *parent = 0);
    QFont font() const
        { return myFont; }
    QColor textColor() const
        { return myTextColor; }
    QColor itemColor() const
        { return myItemColor; }
    QColor lineColor() const
        { return myLineColor; }
    void setLineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setItemColor(const QColor &color);
    void setFont(const QFont &font);

public slots:
    void setMode(Mode mode);
    void setItemType(DiagramItem::DiagramType type);
    void editorLostFocus(DiagramTextItem *item);

signals:
    void itemInserted(DiagramItem *item);	// 不用實作,主要只是用來做signals
    void textInserted(QGraphicsTextItem *item);
    void itemSelected(QGraphicsItem *item);	// 沒有實作

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    bool isItemChange(int type);

    DiagramItem::DiagramType myItemType;	// 目前針對的item的型態:Step, Conditional, StartEnd, Io
    QMenu *myItemMenu;				// 由針對item建立的menu
    Mode myMode;				// 目前scene的模式
    QGraphicsLineItem *line;			// 目前的直線
    QFont myFont;				// 目前的字型
    DiagramTextItem *textItem;			// 記錄字型的item
    QColor myTextColor;				// 字的顏色
    QColor myItemColor;				// 圖型的顏色
    QColor myLineColor;				// 線的顏色
};
//! [0]

#endif
