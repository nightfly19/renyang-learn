#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include "ImageLabel.h"

ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent) {
    this->setAcceptDrops(true);	// 允許拖曳到此widget放下滑鼠
}

void ImageLabel::dragEnterEvent(QDragEnterEvent *event) {
    if(event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();	// 接受,並且是用預設的方式copy,move,LinkAction,ActionMask,IgnoreAction
	// 可以判斷只有某一個方式才能接受
	// if (event->proposedAction() == Qt::CopyAction)
    }
}

void ImageLabel::dropEvent(QDropEvent *event) {
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }
    
    QString fileName = urls.first().toLocalFile();
    if (fileName.isEmpty()) {
        return;
    }
    
    this->setWindowTitle(fileName);
    readImage(fileName);
}

void ImageLabel::readImage(const QString &fileName) {
    QPixmap pixmap(fileName);
    this->setPixmap(pixmap);
    this->resize(pixmap.width(), pixmap.height());
}
