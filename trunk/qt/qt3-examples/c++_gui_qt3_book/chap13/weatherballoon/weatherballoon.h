#ifndef WEATHERBALLOON_H
#define WEATHERBALLOON_H

#include <qpushbutton.h>
#include <qsocketdevice.h>

class WeatherBalloon : public QPushButton
{
    Q_OBJECT
public:
    WeatherBalloon(QWidget *parent = 0, const char *name = 0);

    double temperature() const;
    double humidity() const;
    double altitude() const;

protected:
    void timerEvent(QTimerEvent *event);

private:
    QSocketDevice socketDevice;
    int myTimerId;
};

#endif
