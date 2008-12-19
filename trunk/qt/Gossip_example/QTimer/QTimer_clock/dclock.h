#ifndef DCLOCK_H
#define DCLOCK_H

#include <qlcdnumber.h>

class DigitalClock : public QLCDNumber {
    Q_OBJECT
public:
    DigitalClock(QWidget *parent=0, const char *name=0);

protected:
    void timerEvent(QTimerEvent *);

private slots:
    void showTime();

private:
    bool showingColon;
};

#endif
