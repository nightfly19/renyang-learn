#include <qdatetime.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qsocketnotifier.h>

#include "weatherstation.h"

WeatherStation::WeatherStation(QWidget *parent, const char *name)
    : QDialog(parent, name),
      socketDevice(QSocketDevice::Datagram)
{
    socketDevice.setBlocking(false);
    socketDevice.bind(QHostAddress(), 5824);

    socketNotifier = new QSocketNotifier(socketDevice.socket(),
                                         QSocketNotifier::Read,
                                         this);
    connect(socketNotifier, SIGNAL(activated(int)),
            this, SLOT(dataReceived()));

    dateLabel = new QLabel(tr("Date:"), this);
    timeLabel = new QLabel(tr("Time:"), this);
    temperatureLabel = new QLabel(tr("Temperature:"), this);
    humidityLabel = new QLabel(tr("Humidity:"), this);
    altitudeLabel = new QLabel(tr("Altitude:"), this);

    dateLineEdit = new QLineEdit(this);
    timeLineEdit = new QLineEdit(this);
    temperatureLineEdit = new QLineEdit(this);
    humidityLineEdit = new QLineEdit(this);
    altitudeLineEdit = new QLineEdit(this);

    dateLineEdit->setReadOnly(true);
    timeLineEdit->setReadOnly(true);
    temperatureLineEdit->setReadOnly(true);
    humidityLineEdit->setReadOnly(true);
    altitudeLineEdit->setReadOnly(true);

    setCaption(tr("Weather Station"));

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setResizeMode(QLayout::Fixed);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);
    mainLayout->addWidget(dateLabel, 0, 0);
    mainLayout->addWidget(dateLineEdit, 0, 1);
    mainLayout->addWidget(timeLabel, 1, 0);
    mainLayout->addWidget(timeLineEdit, 1, 1);
    mainLayout->addWidget(temperatureLabel, 2, 0);
    mainLayout->addWidget(temperatureLineEdit, 2, 1);
    mainLayout->addWidget(humidityLabel, 3, 0);
    mainLayout->addWidget(humidityLineEdit, 3, 1);
    mainLayout->addWidget(altitudeLabel, 4, 0);
    mainLayout->addWidget(altitudeLineEdit, 4, 1);
}

void WeatherStation::dataReceived()
{
    QDateTime dateTime;
    double temperature;
    double humidity;
    double altitude;

    QByteArray datagram(socketDevice.bytesAvailable());
    socketDevice.readBlock(datagram.data(), datagram.size());

    QDataStream in(datagram, IO_ReadOnly);
    in.setVersion(5);
    in >> dateTime >> temperature >> humidity >> altitude;

    dateLineEdit->setText(dateTime.date().toString());
    timeLineEdit->setText(dateTime.time().toString());
    temperatureLineEdit->setText(tr("%1 °C").arg(temperature));
    humidityLineEdit->setText(tr("%1%").arg(humidity));
    altitudeLineEdit->setText(tr("%1 m").arg(altitude));
}
