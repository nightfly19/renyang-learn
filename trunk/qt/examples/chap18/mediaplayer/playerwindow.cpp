#include <qaxwidget.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qslider.h>
#include <qstyle.h>
#include <qtimer.h>
#include <qtoolbutton.h>

#include "playerwindow.h"

PlayerWindow::PlayerWindow(QWidget *parent, const char *name)
    : QWidget(parent, name)
{
    setCaption(tr("Media Player"));
    fileFilters = tr("Video files (*.mpg *.mpeg *.avi *.wmv)\n"
                     "Audio files (*.mp3 *.wav)");
    updateTimer = 0;

    setMouseTracking(true);

    wmp = new QAxWidget(this);
    wmp->setControl("{22D6F312-B0F6-11D0-94AB-0080C74C7E95}");
    wmp->setProperty("ShowControls", QVariant(false, 0));
    wmp->setSizePolicy(QSizePolicy::Expanding,
                       QSizePolicy::Expanding);
    connect(wmp, SIGNAL(PlayStateChange(int, int)),
            this, SLOT(onPlayStateChange(int, int)));
    connect(wmp, SIGNAL(ReadyStateChange(ReadyStateConstants)),
            this, SLOT(onReadyStateChange(ReadyStateConstants)));
    connect(wmp, SIGNAL(PositionChange(double, double)),
            this, SLOT(onPositionChange(double, double)));

    openButton = new QToolButton(this);
    openButton->setText(tr("&Open..."));
    connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));

    playPauseButton = new QToolButton(this);
    playPauseButton->setText(tr("&Play"));
    playPauseButton->setEnabled(false);
    connect(playPauseButton, SIGNAL(clicked()), wmp, SLOT(Play()));

    stopButton = new QToolButton(this);
    stopButton->setText(tr("&Stop"));
    stopButton->setEnabled(false);
    connect(stopButton, SIGNAL(clicked()), wmp, SLOT(Stop()));

    seekSlider = new QSlider(Horizontal, this);
    seekSlider->setEnabled(false);
    connect(seekSlider, SIGNAL(valueChanged(int)),
            this, SLOT(sliderValueChanged(int)));
    connect(seekSlider, SIGNAL(sliderPressed()),
            wmp, SLOT(Pause()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(openButton);
    buttonLayout->addWidget(playPauseButton);
    buttonLayout->addWidget(stopButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(wmp);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(seekSlider);
}

void PlayerWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == updateTimer) {
        double curPos = wmp->property("CurrentPosition").toDouble();
        onPositionChange(-1, curPos);
    } else {
        QWidget::timerEvent(event);
    }
}

void PlayerWindow::onPlayStateChange(int, int newState)
{
    playPauseButton->disconnect(wmp);

    switch (newState) {
    case Stopped:
        if (updateTimer) {
            killTimer(updateTimer);
            updateTimer = 0;
        }
        // fall through
    case Paused:
        connect(playPauseButton, SIGNAL(clicked()),
                wmp, SLOT(Play()));
        stopButton->setEnabled(false);
        playPauseButton->setText(tr("&Play"));
        break;
    case Playing:
        if (!updateTimer)
            updateTimer = startTimer(100);
        connect(playPauseButton, SIGNAL(clicked()),
                wmp, SLOT(Pause()));
        stopButton->setEnabled(true);
        playPauseButton->setText(tr("&Pause"));
    }
}

void PlayerWindow::onReadyStateChange(ReadyStateConstants ready)
{
    if (ready == Complete) {
        double duration = 60 * wmp->property("Duration").toDouble();
        seekSlider->setMinValue(0);
        seekSlider->setMaxValue((int)duration);
        seekSlider->setEnabled(true);
        playPauseButton->setEnabled(true);
    }
}

void PlayerWindow::onPositionChange(double, double newPos)
{
    seekSlider->blockSignals(true);
    seekSlider->setValue((int)(newPos * 60));
    seekSlider->blockSignals(false);
}

void PlayerWindow::sliderValueChanged(int newValue)
{
    seekSlider->blockSignals(true);
    wmp->setProperty("CurrentPosition", (double)newValue / 60);
    seekSlider->blockSignals(false);
}

void PlayerWindow::openFile()
{
    QString fileName =
            QFileDialog::getOpenFileName(".", fileFilters, this);
    if (!fileName.isEmpty())
        wmp->setProperty("FileName",
                         QDir::convertSeparators(fileName));
}
