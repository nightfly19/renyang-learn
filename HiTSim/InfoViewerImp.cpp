
#include <qlineedit.h>
#include <qlistbox.h>
#include <qlabel.h>

#include "InfoViewerImp.h"
#include "WifiCar.h"
#include "Car.h"
#include "DSRCCar.h"

InfoViewerImp::InfoViewerImp( QWidget* parent, const char* name, WFlags fl )
    : InfoViewer( parent, name, fl )
    , sel( NULL )
{
	qtimer = new QTimer(this," Timer For InfoViewerImp");

	connect( qtimer , SIGNAL( timeout() ), this , SLOT(slotUpdate()));
	for( int ch = 0 ; ch < CH_MAX ; ch ++ ){
		lb_channels -> insertItem(QString::number(ch),ch);
	}
}

InfoViewerImp::~InfoViewerImp()
{
	delete qtimer;
	sel = NULL;
}
void InfoViewerImp::slotCarSel(Car* c)
{
	if(c)
		debug("InfoViewerImp:: select %d",c->nodeID());
	sel = c;

	slotUpdate();
}

void InfoViewerImp::hide()
{
	qtimer -> stop();
	InfoViewer::hide();
}
void InfoViewerImp::show()
{
	qtimer -> start(100);
	InfoViewer::show();
}

void InfoViewerImp::slotHide()
{
	hide();
}
void InfoViewerImp::slotUpdate()
{
	if( ! sel )	return;
	WifiCar *wc = (WifiCar*)(sel);
	DSRCCar *dc = (DSRCCar*)(sel);

	le_cid -> setText( QString::number( sel -> nodeID() ));
	le_pos -> setText( "(" + QString::number( sel -> realX() , 'f' , 5 ) +" , "
			   + QString::number( sel -> realY()) + " ) " );
	
	switch(sel -> CarType()){
		case CT_WIFI:
			tl_car_type -> setText("Wifi Car");
			wc-> drawTable( lv_poslist , tl_information, lb_channels);
			break;
		case CT_DSRC:
			tl_car_type -> setText("DSRC Car");
			dc-> drawTable( lv_poslist , tl_information, lb_channels );
			break;
		case CT_NORMAL:
		default:
			tl_car_type -> setText("Normal Car");
			break;
	} 
}
