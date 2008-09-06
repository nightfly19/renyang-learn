#ifndef THREAD_H
#define THREAD_H

#include <qthread.h>

class Thread : public QThread
{
public:
    Thread();

    void setMessage(const QString &message);
    void run();
    void stop();

private:
    QString messageStr;
    volatile bool stopped;
};

#endif
