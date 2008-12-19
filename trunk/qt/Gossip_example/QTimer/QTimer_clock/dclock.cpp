#include "dclock.h"
#include <qdatetime.h>

// Debug start
#include <iostream>
using namespace std;
// Debug end

DigitalClock::DigitalClock(QWidget *parent, const char *name)
    : QLCDNumber(parent, name) {
    showingColon = FALSE;
    setFrameStyle(QFrame::Panel | QFrame::Raised);
    setLineWidth(2);
    showTime();
    QObject::startTimer(500);	// 設定QTimer定時發出QTimerEvent事件
}

void DigitalClock::timerEvent(QTimerEvent *e) {
    showTime();
    qDebug("timer event,id %d",e->timerId());	// 可以顯示由哪一個timer觸發timerEvent
}

void DigitalClock::showTime() {
    showingColon = !showingColon;	// 500ms 冒號亮暗一次
    QString s = QTime::currentTime().toString().left(5);
    // start debug
    cout << s << endl;
    // end of debug
    if (!showingColon)	// 決定是否顯示冒號
        s[2] = ' ';
    if (s[0] == '0')
        s[0] = ' ';
    display(s);
}
