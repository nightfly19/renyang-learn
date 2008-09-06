
/****************************************************************
**
** Definition of CannonField class, Qt tutorial 12
**
****************************************************************/

#ifndef CANNON_H
#define CANNON_H

class QTimer;


#include <qwidget.h>

enum SHOOT_MODE {shoot_mode,cheat_mode};


class CannonField : public QWidget
{
    Q_OBJECT
public:
    CannonField( QWidget *parent=0, const char *name=0 );

    int   angle() const { return ang; }
    int   force() const { return f; }
    QSizePolicy sizePolicy() const;

public slots:
    void  setAngle( int degrees );
    void  setForce( int newton );
    void  shoot();
    void  newTarget();
    // homework-1-begin
    void cheat_shoot();
    // homework-1-end

private slots:
    void  moveShot();

signals:
    void  hit();
    void  missed();
    void  angleChanged( int );
    void  forceChanged( int );

protected:
    void  paintEvent( QPaintEvent * );

private:
    void  paintShot( QPainter * );
    void  paintTarget( QPainter * );
    void  paintCannon( QPainter * );
    QRect cannonRect() const;
    QRect shotRect() const;
    QRect targetRect() const;
    void move();

    int ang;
    int f;

    int timerCount;
    QTimer * autoShootTimer;
    float shoot_ang;
    float shoot_f;

    QPoint target;
    SHOOT_MODE mode;
};


#endif // CANNON_H

