#ifndef AXBOUNCER_H
#define AXBOUNCER_H

#include <qaxbindable.h>
#include <qwidget.h>

class AxBouncer : public QWidget, public QAxBindable
{
    Q_OBJECT
    Q_ENUMS(Speed)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(Speed speed READ speed WRITE setSpeed)
    Q_PROPERTY(int radius READ radius WRITE setRadius)
    Q_PROPERTY(bool running READ isRunning)

public:
    enum Speed { Slow, Normal, Fast };

    AxBouncer(QWidget *parent = 0, const char *name = 0);

    void setSpeed(Speed newSpeed);
    Speed speed() const { return ballSpeed; }
    void setRadius(int newRadius);
    int radius() const { return ballRadius; }
    void setColor(const QColor &newColor);
    QColor color() const { return ballColor; }
    bool isRunning() const { return myTimerId != 0; }
    QSize sizeHint() const;
    QAxAggregated *createAggregate();

public slots:
    void start();
    void stop();

signals:
    void bouncing();

protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);

private:
    int intervalInMilliseconds() const;

    QColor ballColor;
    Speed ballSpeed;
    int ballRadius;
    int myTimerId;
    int x;
    int delta;
};

#endif
