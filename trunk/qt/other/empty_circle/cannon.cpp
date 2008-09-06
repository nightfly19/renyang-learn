
/****************************************************************
**
** Implementation CannonField class, Qt tutorial 8
**
****************************************************************/

#include "cannon.h"
#include <qpainter.h>


CannonField::CannonField( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    ang = 45;
    circle_x=0;
    setPalette( QPalette( QColor( 250, 250, 200) ) );
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


void CannonField::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    p.setBrush( NoBrush );
    p.setPen( red );
    p.drawEllipse(circle_x,0,120,120);
    s = "Angle = " + QString::number(ang);
    p.setPen( SolidLine );
    p.drawText(rect(),AlignCenter,s);

}


QSizePolicy CannonField::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

