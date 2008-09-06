#ifndef TRANSACTIONTHREAD_H
#define TRANSACTIONTHREAD_H

#include <qimage.h>
#include <qmutex.h>
#include <qthread.h>

#include <list>

class Transaction
{
public:
    virtual QImage apply(const QImage &image) = 0;
    virtual QString messageStr() = 0;
};

class FlipTransaction : public Transaction
{
public:
    FlipTransaction(Qt::Orientation orient);

    QImage apply(const QImage &image);
    QString messageStr();

private:
    Qt::Orientation orientation;
};

class ResizeTransaction : public Transaction
{
public:
    ResizeTransaction(const QSize &newSize);

    QImage apply(const QImage &image);
    QString messageStr();

private:
    QSize size;
};

class ConvertDepthTransaction : public Transaction
{
public:
    ConvertDepthTransaction(int newDepth);

    QImage apply(const QImage &image);
    QString messageStr();

private:
    int depth;
};

enum { TransactionStart = 1001, AllTransactionsDone = 1002 };

class TransactionStartEvent : public QCustomEvent
{
public:
    TransactionStartEvent();

    QString message;
};

class TransactionThread : public QThread
{
public:
    void run();
    void setTargetWidget(QWidget *widget);
    void addTransaction(Transaction *transact);
    void setImage(const QImage &image);
    QImage image();

private:
    QWidget *targetWidget;
    QMutex mutex;
    QImage currentImage;
    std::list<Transaction *> transactions;
};

#endif
