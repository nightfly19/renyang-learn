
#include "global.h"
#include "BouncyLogo.h"

int BouncyLogo::rtti() const
{
    return LOGO_RTTI;	// the LOGO_RTTI declare in the gloal.h
}

void BouncyLogo::initPos()
{
    initSpeed();
    int trial=1000;
    do {
	move(rand()%canvas()->width(),rand()%canvas()->height());
	advance(0);
    } while (trial-- && xVelocity()==0.0 && yVelocity()==0.0);
}

void BouncyLogo::initSpeed()
{
/*
    const double speed = 4.0;
    double d = (double)(rand()%1024) / 1024.0;
    //setVelocity( d*speed*2-speed, (1-d)*speed*2-speed );
    setVelocity( d*speed*2-speed, 0 );
    */
}

void BouncyLogo::advance(int stage)
{
    switch ( stage ) {
      case 0: 
//        setX( x() - 1 );
//	  do nothing
	break;
      case 1:
	QCanvasItem::advance(stage);
	break;
    }
}



BouncyLogo::BouncyLogo(QCanvas* canvas) :
    QCanvasSprite(0,canvas)
{
//    static QCanvasPixmapArray logo("car-2.png");
//    setSequence(&logo);
    setAnimated(TRUE);
    initPos();
}

/*
 
void BouncyLogo::advance(int stage)
{
    switch ( stage ) {
      case 0: {
	double vx = xVelocity();
	double vy = yVelocity();

	if ( vx == 0.0 && vy == 0.0 ) {
	    // stopped last turn
	    initSpeed();
	    vx = xVelocity();
	    vy = yVelocity();
	}

	double nx = x() + vx;
	double ny = y() + vy;

	if ( nx < 0 || nx >= canvas()->width() )
	    vx = -vx;
	if ( ny < 0 || ny >= canvas()->height() )
	    vy = -vy;

	for (int bounce=0; bounce<4; bounce++) {
	    QCanvasItemList l=collisions(FALSE);
	    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
		QCanvasItem *hit = *it;
		if ( hit->rtti()==LOGO_RTTI && hit->collidesWith(this) ) {
		    switch ( bounce ) {
		      case 0:
			vx = -vx;
			break;
		      case 1:
			vy = -vy;
			vx = -vx;
			break;
		      case 2:
			vx = -vx;
			break;
		      case 3:
			// Stop for this turn
			vx = 0;
			vy = 0;
			break;
		    }
		    setVelocity(vx,vy);
		    break;
		}
	    }
	}

	if ( x()+vx < 0 || x()+vx >= canvas()->width() )
	    vx = 0;
	if ( y()+vy < 0 || y()+vy >= canvas()->height() )
	    vy = 0;

	setVelocity(vx,vy);
      } break;
      case 1:
	QCanvasItem::advance(stage);
	break;
    }
}

*/
