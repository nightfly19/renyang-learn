#ifndef WEATHERSTATION_H
#define WEATHERSTATION_H

#include <qdialog.h>
#include <qsocketdevice.h>

class QLabel;
class QLineEdit;
class QSocketNotifier;

class WeatherStation : public QDialog
{
    Q_OBJECT
public:
    WeatherStation(QWidget *parent = 0, const char *name = 0);

private slots:
    void dataReceived();

private:
    QSocketDevice socketDevice;
    QSocketNotifier *socketNotifier;

    QLabel *dateLabel;
    QLabel *timeLabel;
    QLabel *temperatureLabel;
    QLabel *humidityLabel;
    QLabel *altitudeLabel;
    QLineEdit *dateLineEdit;
    QLineEdit *timeLineEdit;
    QLineEdit *temperatureLineEdit;
    QLineEdit *humidityLineEdit;
    QLineEdit *altitudeLineEdit;
};

#endif
