#include <qapplication.h>

#include "transactionthread.h"

using namespace std;

FlipTransaction::FlipTransaction(Qt::Orientation orient)
{
    orientation = orient;
}

QImage FlipTransaction::apply(const QImage &image)
{
    return image.mirror(orientation == Qt::Horizontal,
                        orientation == Qt::Vertical);
}

QString FlipTransaction::messageStr()
{
    if (orientation == Qt::Horizontal)
        return QObject::tr("Flipping image horizontally...");
    else
        return QObject::tr("Flipping image vertically...");
}

ResizeTransaction::ResizeTransaction(const QSize &newSize)
{
    size = newSize;
}

QString ResizeTransaction::messageStr()
{
    return QObject::tr("Resizing image...");
}

QImage ResizeTransaction::apply(const QImage &image)
{
    return image.smoothScale(size);
}

ConvertDepthTransaction::ConvertDepthTransaction(int newDepth)
{
    depth = newDepth;
}

QImage ConvertDepthTransaction::apply(const QImage &image)
{
    return image.convertDepth(depth);
}

QString ConvertDepthTransaction::messageStr()
{
    return QObject::tr("Converting image depth...");
}

TransactionStartEvent::TransactionStartEvent()
    : QCustomEvent(TransactionStart)
{
}

void TransactionThread::setTargetWidget(QWidget *widget)
{
    targetWidget = widget;
}

void TransactionThread::addTransaction(Transaction *transact)
{
    QMutexLocker locker(&mutex);
    transactions.push_back(transact);
    if (!running())
        start();
}

void TransactionThread::run()
{
    Transaction *transact;

    for (;;) {
        mutex.lock();
        if (transactions.empty()) {
            mutex.unlock();
            break;
        }
        QImage oldImage = currentImage;
        transact = *transactions.begin();
        transactions.pop_front();
        mutex.unlock();

        TransactionStartEvent *event = new TransactionStartEvent;
        event->message = transact->messageStr();
        QApplication::postEvent(targetWidget, event);

        QImage newImage = transact->apply(oldImage);
        delete transact;

        mutex.lock();
        currentImage = newImage;
        mutex.unlock();
    }
    QApplication::postEvent(targetWidget,
                            new QCustomEvent(AllTransactionsDone));
}

void TransactionThread::setImage(const QImage &image)
{
    QMutexLocker locker(&mutex);
    currentImage = image.copy();
}

QImage TransactionThread::image()
{
    QMutexLocker locker(&mutex);
    return currentImage.copy();
}
