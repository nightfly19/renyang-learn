#include "ListWidget.h"

#include <QApplication>
#include <QPoint>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QListWidgetItem>
#include <QIcon>

ListWidget::ListWidget(QWidget *parent) : QListWidget(parent) {
    setAcceptDrops(true);
}

void ListWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        startPoint = event->pos();
    }
    QListWidget::mousePressEvent(event);
}

void ListWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        if ((event->pos() - startPoint).manhattanLength()
                >= QApplication::startDragDistance()) {	// 設定拖曳多遠會出現拖曳的小圖
            execDrag();
        }
    }
    QListWidget::mouseMoveEvent(event);
}

void ListWidget::execDrag() {
    QListWidgetItem *item = currentItem();
    if (item) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->text());
        mimeData->setImageData(item->icon());
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(item->icon().pixmap(QSize(22, 22)));
        if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
            delete item;
        }
    }
}

void ListWidget::dragEnterEvent(QDragEnterEvent *event) {
    ListWidget *source =
            qobject_cast<ListWidget *>(event->source());
    if (source && source != this) {	// 這裡就是在判斷是否不是自己
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void ListWidget::dragMoveEvent(QDragMoveEvent *event) {}

void ListWidget::dropEvent(QDropEvent *event) {
    ListWidget *source =
            qobject_cast<ListWidget *>(event->source());
    if (source && source != this) {        
        QIcon icon = event->mimeData()->imageData().value<QIcon>();
        QString text = event->mimeData()->text(); 
        addItem(new QListWidgetItem(icon, text));
        
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}
