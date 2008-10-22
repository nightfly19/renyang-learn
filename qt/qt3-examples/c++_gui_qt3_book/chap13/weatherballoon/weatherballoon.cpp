#include <qdatetime.h>
#include <qhostaddress.h>

#include <cstdlib>
using namespace std;

#include "weatherballoon.h"

WeatherBalloon::WeatherBalloon(QWidget *parent, const char *name)
    : QPushButton(tr("Quit"), parent, name),
      socketDevice(QSocketDevice::Datagram)
{
    socketDevice.setBlocking(false);
    myTimerId = startTimer(5 * 1000);
}

void WeatherBalloon::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == myTimerId) {
        QByteArray datagram;
        QDataStream out(datagram, IO_WriteOnly);
        out.setVersion(5);
        out << QDateTime::currentDateTime() << temperature()
            << humidity() << altitude();
        socketDevice.writeBlock(datagram, datagram.size(),
                                0x7F000001, 5824);
    } else {
        QPushButton::timerEvent(event);
    }
}

double WeatherBalloon::temperature() const
{
    return -20.0 + (2.0 * rand() / (RAND_MAX + 1.0));
}

double WeatherBalloon::humidity() const
{
    return 20.0 + (2.0 * rand() / (RAND_MAX + 1.0));
}

double WeatherBalloon::altitude() const
{
    return 7000 + (100.0 * rand() / (RAND_MAX + 1.0));
}
