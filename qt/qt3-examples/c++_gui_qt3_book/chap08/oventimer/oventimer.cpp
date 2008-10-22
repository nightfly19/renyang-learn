#include <qpainter.h>
#include <qpixmap.h>
#include <qtimer.h>

#include <cmath>
using namespace std;

#include "oventimer.h"

const double DegreesPerMinute = 7.0;
const double DegreesPerSecond = DegreesPerMinute / 60;
const int MaxMinutes = 45;
const int MaxSeconds = MaxMinutes * 60;
const int UpdateInterval = 10;

OvenTimer::OvenTimer(QWidget *parent, const char *name)
    : QWidget(parent, name, WNoAutoErase)
{
    finishTime = QDateTime::currentDateTime();
    updateTimer = new QTimer(this);
    finishTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    connect(finishTimer, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

void OvenTimer::setDuration(int secs)
{
    if (secs > MaxSeconds)
        secs = MaxSeconds;
    finishTime = QDateTime::currentDateTime().addSecs(secs);
    updateTimer->start(UpdateInterval * 1000, false);
    finishTimer->start(secs * 1000, true);
    update();
}

int OvenTimer::duration() const
{
    int secs = QDateTime::currentDateTime().secsTo(finishTime);
    if (secs < 0)
        secs = 0;
    return secs;
}

void OvenTimer::mousePressEvent(QMouseEvent *event)
{
    QPoint point = event->pos() - rect().center();
    double theta = atan2(-(double)point.x(), -(double)point.y())
                   * 180 / 3.14159265359;
    setDuration((int)(duration() + theta / DegreesPerSecond));
    update();
}

void OvenTimer::paintEvent(QPaintEvent *event)
{
    static QPixmap pixmap;
    QRect rect = event->rect();

    QSize newSize = rect.size().expandedTo(pixmap.size());
    pixmap.resize(newSize);
    pixmap.fill(this, rect.topLeft());

    QPainter painter(&pixmap, this);
    int side = QMIN(width(), height());
    painter.setViewport((width() - side) / 2 - event->rect().x(),
                        (height() - side) / 2 - event->rect().y(),
                        side, side);
    painter.setWindow(-50, -50, 100, 100);
    draw(&painter);
    bitBlt(this, event->rect().topLeft(), &pixmap);
}

void OvenTimer::draw(QPainter *painter)
{
    static const QCOORD triangle[3][2] = {
        { -2, -49 }, { +2, -49 }, { 0, -47 }
    };
    QPen thickPen(colorGroup().foreground(), 2);
    QPen thinPen(colorGroup().foreground(), 1);

    painter->setPen(thinPen);
    painter->setBrush(colorGroup().foreground());
    painter->drawConvexPolygon(QPointArray(3, &triangle[0][0]));

    painter->setPen(thickPen);
    painter->setBrush(colorGroup().light());
    painter->drawEllipse(-46, -46, 92, 92);
    painter->setBrush(colorGroup().mid());
    painter->drawEllipse(-20, -20, 40, 40);
    painter->drawEllipse(-15, -15, 30, 30);

    int secs = duration();
    painter->rotate(secs * DegreesPerSecond);
    painter->drawRect(-8, -25, 16, 50);

    for (int i = 0; i <= MaxMinutes; ++i) {
        if (i % 5 == 0) {
            painter->setPen(thickPen);
            painter->drawLine(0, -41, 0, -44);
            painter->drawText(-15, -41, 30, 25,
                              AlignHCenter | AlignTop,
                              QString::number(i));
        } else {
            painter->setPen(thinPen);
            painter->drawLine(0, -42, 0, -44);
        }
        painter->rotate(-DegreesPerMinute);
    }
}
