
/****************************************************************
**
** Implementation CannonField class, Qt tutorial 8
**
****************************************************************/

#include "cannon.h"
#include <qpainter.h>
#include <qtimer.h>
#include <qcanvas.h>


CannonField::CannonField( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    ang = 45;
    circle_x=0;
    setPalette( QPalette( QColor( 250, 250, 200) ) );
    QTimer *time = new QTimer(this,"Handler");
    connect(time,SIGNAL(timeout()),SLOT(move_circle()));
    time->start(1000);
}


void CannonField::setAngle( int degrees )
{
    if ( degrees < 5 )
        degrees = 5;
    if ( degrees > 70 )
        degrees = 70;
    if ( ang == degrees )
        return;
    ang = degrees;
    repaint();
    emit angleChanged( ang );
}

void CannonField::move_circle()
{
	circle_x+=10;
	repaint();
}

void CannonField::paintEmptyCircle(QPainter *p)
{
	p->setBrush(NoBrush);
	p->setPen(SolidLine);
	p->drawEllipse(shotRect());
}

void CannonField::paintTarget(QPainter *p)
{
	p->setBrush(SolidPattern);
	p->setPen(NoPen);
	p->drawRect(targetRect());
}

void CannonField::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    paintEmptyCircle(&p);
    paintTarget(&p);
    if (shotRect().intersects(targetRect()))
    	s = "Collision";
    else
    	s = "Angle = " + QString::number(ang);
    p.drawText(rect(),AlignCenter,s);

}

QRect CannonField::shotRect()
{
	QRect shot(circle_x,0,120,120);
	return shot;
}

QRect CannonField::targetRect()
{
	QRect shot(200,100,20,20);
	return shot;
}

QSizePolicy CannonField::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

