
#include <qdatetime.h>
#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qaction.h>
#include <qlabel.h>
#include <qimage.h>
#include <qlineedit.h>
#include <qprogressdialog.h>
#include <qpopupmenu.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qscrollbar.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include <qfiledialog.h>

#include "dlg/dlgsimsetting.h"
#include "dlg/dlgcarsetting.h"
#include "dlg/initial_set.h"
#include "dlg/initial_set_dsrc.h"
#include "MainWindow.h"
#include "InfoViewerImp.h"
#include "ImageItem.h"
#include "BouncyLogo.h"
#include "global.h"
#include "canvas.h"
#include "Coordinator.h"
#include "network/net_coor.h"
#include "Car.h"
#include "WifiCar.h"
#include "DSRCCar.h"
#include "Road.h"

static uint mainCount = 0;
static QImage *butterflyimg;
static QImage *logoimg;

static int mod7 = 70;
Main::Main(QCanvas& c, QWidget* parent, const char* name, WFlags f) 
    //QMainWindow(parent,name,f),
    : cMainWindow(parent,name,f)
    , canvas(c)
    , ui_state(UI_USER_SEL)
{
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );
    QGridLayout* cMainWindowLayout = new QGridLayout( centralWidget(), 1, 1, 11, 6, "cMainWindowLayout");
    QHBoxLayout* layout1 = new QHBoxLayout( 0, 0, 6, "layout1");
    QVBoxLayout* layout2 = new QVBoxLayout( 0, 0, 6, "layout1");


    editor = new FigureEditor(canvas,centralWidget(), "Main Figure Editor" );
    editor -> setFocusPolicy(StrongFocus);
    centralWidget() -> setFocusProxy(editor);
    coordinator = new Coordinator(canvas, this,"Car Coordinator");
    net_coor= new NetCoordinator(this,"Wifi Network Coordinator");


    _sel_car = NULL;
	
    sc_position = new QScrollBar(Qt::Horizontal,centralWidget());
//    statusBar() -> addWidget(sc_position,1);
    sc_position -> setFocusProxy(editor);
    sc_position->setMinValue(gStartPosition*1000*10);
    sc_position->setMaxValue(gEndPosition*1000*10);
    pb_center = new QPushButton( centralWidget(), "pb_center" );
    pb_center->setMaximumSize( QSize( 40, 40) );
    pb_center->setToggleButton( TRUE );

    pos_viewer = new InfoViewerImp(NULL);

// 設定Layout
    layout1->addWidget( sc_position );
    layout1->addWidget( pb_center );
    layout2->addWidget( editor );
    layout2->addLayout( layout1 );
    cMainWindowLayout->addLayout( layout2, 0, 0 );

   // statusBar(); 
//    setCentralWidget(editor); 
    printer = 0; 
    init();

//  產生Road
    road = new Road(&canvas);
    road -> setZ(-100);
    road -> show();
    coordinator -> setRoad(road);

    languageChange();

    connect( editor , SIGNAL( leftSelect(QCanvasItem*) ), this, SLOT( slotLeftSelect(QCanvasItem*) ) );
    connect( editor , SIGNAL( rightSelect(QCanvasItem*) ), this, SLOT( slotRightSelect(QCanvasItem*) ) );
    connect( editor , SIGNAL( sigMouseRelease() ), this, SLOT( slotMouseRelease() ) );
    connect( sc_position , SIGNAL( valueChanged(int) ), this, SLOT(slotScrollPosition(int)) );
    connect( pb_center, SIGNAL( toggled(bool) ), this, SLOT(slotSetFixedCenter(bool)) );
    connect( this , SIGNAL(CarSel(Car*)) , pos_viewer , SLOT(slotCarSel(Car*)));
    connect( coordinator , SIGNAL(simulationDone()), this , SLOT(slotSimDone()));
//    connect( editor , SIGNAL( sgUpdateEnable(bool) ), editor, SLOT( setUpdatesEnabled(bool) ) );
//

	QTimer *qtimer = new QTimer(this, "Timer for Mainwindow");
	qtimer -> start( 100 );//目前用這一個變數應該就夠了
	connect( qtimer , SIGNAL(timeout()), this , SLOT(slotUpdateStatusBar() ) );
}

void Main::languageChange()
{

    pb_center->setText( tr( "C" ) );
}

void Main::slotUpdateStatusBar()
{
	Car* car = coordinator->CenterCar();
	unsigned int tmp_count = coordinator->CountCollision();
	QString str = "Time : "+ QString::number(g_timer_ticks/1000.0,'f',2 )
		+ " Network Collision : "+QString::number(g_net_collision/mod7)
		+ " Car Collision : "+QString::number(g_car_collision)
		+ "/"+QString::number(tmp_count)
		+ " Average Velocity : "+QString::number(coordinator->CountCarVelocity())
		;
	if(car)
		str+= " Center Car ID :" +QString::number( car->nodeID() );
	showMsg(str);

	tryWriteLogFile();
}

void Main::showMsg(QString & str )
{
	statusBar()->message(str);
}

void Main::init()
{
    slotClear();

   // static int r=24;
    //srand(++r);
    srand( time(NULL) );

    mainCount++;
    butterflyimg = 0;
    logoimg = 0;

    // Reset View Point
    QWMatrix m = editor->worldMatrix();
    m.scale( 1/2.0,1/2.0 );
    editor->setWorldMatrix( m );
}


void Main::slotLeftSelect( QCanvasItem* it) // now is only Car
{
	coordinator -> clearCarsFlags( );
	if ( it->rtti() == CAR_RTTI ) {
		switch( ui_state ){ 
			case UI_USER_SEL: {
				Car* car= (Car*)it;
				car -> setSel(true);
				_sel_car = car;

				//WifiCar* wc = (WifiCar*) _sel_car;
				emit CarSel( _sel_car );
				break;
			}
			case UI_CENTER_SEL: {
				Car* car= (Car*)it;
				coordinator->setCenterCar( car );
				//slotSetFixedCenter(true);
				break;
			}
		}
	}
}

void Main::slotRightSelect( QCanvasItem* c)
{
	if ( c ->rtti() == CAR_RTTI) {
		Car* car= (Car*)(c);

		DlgCarSetting dlg(this);

		dlg . setValue( int(car->realVelocity()) , car->maxRealVelocity() );
		dlg . le_node_id -> setText( QString::number(car->nodeID()) );
		
		switch (car->DriverBehavior()) {
		    case BCTRL_MODE:
		       dlg . sp_set_driver_beh -> setCurrentItem(0);
		       break;
		    case CONSERVATIVE:
		       dlg . sp_set_driver_beh -> setCurrentItem(1);
		       break;
		    case NORMAL:
		       dlg . sp_set_driver_beh -> setCurrentItem(2);
		       break;
		    case AGGRESSIVE:
		       dlg . sp_set_driver_beh -> setCurrentItem(3);
		       break;
		}
		
		/*
		switch (car->AccLevel()) {
		    case Car::ACC:
		       dlg . sp_set_acc -> setCurrentItem(0);
		       break;
		    case Car::ACC_REL:
		       dlg . sp_set_acc -> setCurrentItem(1);
		       break;
		    case Car::DEC:
		       dlg . sp_set_acc -> setCurrentItem(2);
		       break;
		    case Car::BRAKE:
		       dlg . sp_set_acc -> setCurrentItem(3);
		       break;
		    case Car::ACTRL_MODE:
		       dlg . sp_set_acc -> setCurrentItem(4);
		       break;
		}*/
		dlg . sp_set_acc -> setCurrentItem(car->AccLevel());
		
		if ( dlg.exec() == QDialog::Accepted ) {
			car -> setRealVelocity( dlg . sp_velocity -> value() );
			car -> setMaxRealVelocity( dlg . sp_max_velocity -> value() );
			car -> setDriverBehavior( dlg . sp_set_driver_beh -> currentItem());
			car -> setAccLevel( dlg . sp_set_acc -> currentItem());
		}
	} 
}


void Main::slotMouseRelease()
{
	
}

Main::~Main()
{
    delete printer;
    delete coordinator;
    delete net_coor;

    pos_viewer -> close();
    delete pos_viewer;

    if ( !--mainCount ) {
	delete[] butterflyimg;
	butterflyimg = 0;
	delete[] logoimg;
	logoimg = 0;
    }
}

void Main::slotNewView()
{
    // Open a new view... have it delete when closed.
    Main *m = new Main(canvas, 0, 0, WDestructiveClose);
    qApp->setMainWidget(m);
    m->show();
    qApp->setMainWidget(0);
}

void Main::slotClear()
{
    editor->clear();
}

void Main::helpAbout()
{
    static QMessageBox* about = new QMessageBox( "Qt Canvas Example",
	    "<h3>The QCanvas classes example</h3>"
	    "<ul>"
		"<li> Press ALT-S for some sprites."
		"<li> Press ALT-C for some circles."
		"<li> Press ALT-L for some lines."
		"<li> Drag the objects around."
		"<li> Read the code!"
	    "</ul>", QMessageBox::Information, 1, 0, 0, this, 0, FALSE );
    about->setButtonText( 1, "Dismiss" );
    about->show();
}

void Main::toggleDoubleBuffer(bool s)
{
    canvas.setDoubleBuffering(s);
}

void Main::slotEnlarge()
{
    canvas.resize(canvas.width()*4/3, canvas.height()*4/3);
    roadResize();
}

void Main::slotShrink()
{
    canvas.resize(canvas.width()*3/4, canvas.height()*3/4);
    roadResize();
}

void Main::roadResize()
{
//	road->setSize(canvas.width(),canvas.height());
}

void Main::rotateClockwise()
{
    QWMatrix m = editor->worldMatrix();
    m.rotate( 22.5 );
    editor->setWorldMatrix( m );
}

void Main::rotateCounterClockwise()
{
    QWMatrix m = editor->worldMatrix();
    m.rotate( -22.5 );
    editor->setWorldMatrix( m );
}

void Main::slotZoomIn()
{
    QWMatrix m = editor->worldMatrix();
    m.scale( 4/3.0, 4/3.0 );
    editor->setWorldMatrix( m );
}

void Main::slotZoomOut()
{
    QWMatrix m = editor->worldMatrix();
    m.scale( 3/4.0, 3/4.0 );
    editor->setWorldMatrix( m );
}

void Main::mirror()
{
    QWMatrix m = editor->worldMatrix();
    m.scale( -1, 1 );
    editor->setWorldMatrix( m );
}

void Main::moveL()
{
    QWMatrix m = editor->worldMatrix();
    m.translate( -16, 0 );
    editor->setWorldMatrix( m );
}

void Main::moveR()
{
    QWMatrix m = editor->worldMatrix();
    m.translate( +16, 0 );
    editor->setWorldMatrix( m );
}

void Main::moveU()
{
    QWMatrix m = editor->worldMatrix();
    m.translate( 0, -16 );
    editor->setWorldMatrix( m );
}

void Main::moveD()
{
    QWMatrix m = editor->worldMatrix();
    m.translate( 0, +16 );
    editor->setWorldMatrix( m );
}

void Main::print()
{
    if ( !printer ) printer = new QPrinter;
    if ( printer->setup(this) ) {
	QPainter pp(printer);
	canvas.drawArea(QRect(0,0,canvas.width(),canvas.height()),&pp,FALSE);
    }
}


void Main::slotAddSprite()
{
    QCanvasItem* i = new BouncyLogo(&canvas);
    i->setZ(rand()%256);
    i->show();

//    coordinator -> add2Coordinator( i );
}

void Main::slotAddCar()
{
	if(coordinator -> CarCount() <= CAR_MX){
		QCanvasItem* i = new Car(&canvas);
		i->setZ(rand()%256);
		randCarPos((Car*)i);
		i->show();
//		i->setDriverBehavior(x);

		coordinator -> add2Coordinator( i );
	}else
		debug("number of Cars is fully!");

}

void Main::slotScrollPosition(int pos)
{
//	debug("Main::slotScrollPosition %d",pos);
	pb_center->setOn(false);
	coordinator-> slotSetScollPosition( pos);
	
	//coordinator->MoveCarsPosition();
	coordinator -> car_active();

	road -> setIfCenter( false );
	road -> setXPos( pos );

	editor->update();
}

void Main::slotSetFixedCenter(bool b)
{
	coordinator->setFixedCenter(b);
	
	if(coordinator->CenterCar()){
		disconnect( sc_position , SIGNAL( valueChanged(int) ), this, SLOT(slotScrollPosition(int)) );
		sc_position->setValue( (int) (coordinator->CenterCar()->realX()-gEndPosition * 1000 * 10));
		connect( sc_position , SIGNAL( valueChanged(int) ), this, SLOT(slotScrollPosition(int)) );
	}
	road -> setIfCenter( b );
}

void Main::randCarPos(Car* car)
{
	static bool run_once = true;
	static int center_x = canvas.width()/2; 
	static int center_y = canvas.height()/2-10; 
	int center_real_y = 0; 
	// init Y
	switch((rand()%(3))){
		case 0:
			car->setRealY(100);
			car->setLan(1);
			break; 
		case 1: 
			car->setRealY(148);
			car->setLan(2);
			break;
		case 2:
		default:
			car->setRealY(196);
			car->setLan(3);
			break;
	}
	// init X
	unsigned int start_position = gStartPosition * 1000 * 10; //pixel
	unsigned int end_position = gEndPosition * 1000 * 10;
	car -> setRealX( (rand()%(start_position-end_position+1))+end_position);
	

	// set Render Pos
	if( run_once ){
		// center car
		car->setX(center_x);
		car->setY(center_y);
		car->setRealY(center_real_y = 148);
		run_once = false;
		sc_position->setValue( (int)(car->realX()-end_position));
	}
	return;
}

void Main::slotRandAddWifiCars()
{
	bool b_auto_change_lane = false;
	Initial_Set dlg(this);

	//    dlg . setValue( car->realVelocity() , car->maxRealVelocity() );
	//    dlg . le_node_id -> setText( QString::number(car->nodeID()) );

	if ( dlg.exec() == QDialog::Accepted ) {
		gCarAmount = dlg . sp_veh_amount -> value();
		gStartPosition = dlg . sp_veh_start_position -> value();
		gEndPosition = dlg . sp_veh_end_position -> value();

		gMaxVelocity = dlg . sp_max_velocity -> value();
		gMinVelocity = dlg . sp_min_velocity -> value();
		gFrictionFactorNormal = dlg . sp_friction_factor -> value();
		gFrictionFactorEmerg = dlg . sp_friction_factor_em -> value();
		//gSafeDistance = dlg . sp_safe_distance -> value();

		gHumanResponseTime = dlg . sp_human_response_time -> value();
		//gSensorDetectionTime = dlg . sp_sensor_detect_time -> value();
		gSystemHandleTime = dlg . sp_system_handle_time -> value();
		//gMachineWorkTime = dlg . sp_machine_work_time -> value();
		//gTimeSlot = dlg . sp_time_slot -> value();

		if( dlg . cb_auto_change_lane -> currentItem() == 0 ){
			b_auto_change_lane = true;
		}else {
			b_auto_change_lane = false; 
		}

//		gTr = (gHumanResponseTime+gSensorDetectionTime+gSystemHandleTime+gMachineWorkTime) * 0.001;
		resetScrollBar();

		//從這邊加入車子       
		for (int a=0; a<gCarAmount; a++) {
			WifiCar* wc = new WifiCar(&canvas);
			wc->setZ(rand()%256);
			randCarPos((Car*)wc);
			wc->show();

			coordinator -> add2Coordinator( wc );
			wc -> setNetCoor( net_coor );
			wc -> setAutoChangeLane( b_auto_change_lane );
		}
		coordinator->setFixedCenter(true);
		//coordinator->MoveCarsPosition();
		coordinator->car_active();
		/* update Scroll Bar 的數值 */
	} 

	mod7 = 10;
}

#include "dlg/settimeratio.h"
void Main::slotSetTimeRatio( )
{
	bool ok=0;
	int ratio = 0;

	SetTimeRatio dlg(this);
	ratio = int (gTimeRatio * 1000);
	debug("Ratio = %d\n", ratio);

	switch (ratio)
	{
		case 1000:
			dlg . sp_time_ratio -> setCurrentItem(0);
			break;
		case 500:
			dlg . sp_time_ratio -> setCurrentItem(1);
			break;
		case 100:
			dlg . sp_time_ratio -> setCurrentItem(2);
			break;
		case 50:
			dlg . sp_time_ratio -> setCurrentItem(3);
			break;
		case 10:
			dlg . sp_time_ratio -> setCurrentItem(4);
			break;
		case 5:
			dlg . sp_time_ratio -> setCurrentItem(5);
			break;
		case 1:
       	       dlg . sp_time_ratio -> setCurrentItem(6);
	       break;

	    default:
       	       dlg . sp_time_ratio -> setCurrentItem(0);
	       break;
        }


    if ( dlg.exec() == QDialog::Accepted ) {
       	gTimeRatio = dlg . sp_time_ratio -> currentText() . toFloat(&ok);

/*
	if(ok)
		printf("ok = true\n");
        
	printf("gTimeRatio = %e\n", gTimeRatio);
        debug("sp time = '"+ dlg . sp_time_ratio -> currentText()+"'");
	*/

    }

//    debug("alotSetTimeRatio");
}

/*
QString butterfly_fn;
QString logo_fn;


void Main::addButterfly()
{
    if ( butterfly_fn.isEmpty() )
	return;
    if ( !butterflyimg ) {
	butterflyimg = new QImage[4];
	butterflyimg[0].load( butterfly_fn );
	butterflyimg[1] = butterflyimg[0].smoothScale( int(butterflyimg[0].width()*0.75),
		int(butterflyimg[0].height()*0.75) );
	butterflyimg[2] = butterflyimg[0].smoothScale( int(butterflyimg[0].width()*0.5),
		int(butterflyimg[0].height()*0.5) );
	butterflyimg[3] = butterflyimg[0].smoothScale( int(butterflyimg[0].width()*0.25),
		int(butterflyimg[0].height()*0.25) );
    }
    QCanvasPolygonalItem* i = new ImageItem(butterflyimg[rand()%4],&canvas);
    i->move(rand()%(canvas.width()-butterflyimg->width()),
	    rand()%(canvas.height()-butterflyimg->height()));
    i->setZ(rand()%256+250);
    i->show();
}

void Main::addLogo()
{
    if ( logo_fn.isEmpty() )
	return;
    if ( !logoimg ) {
	logoimg = new QImage[4];
	logoimg[0].load( logo_fn );
	logoimg[1] = logoimg[0].smoothScale( int(logoimg[0].width()*0.75),
		int(logoimg[0].height()*0.75) );
	logoimg[2] = logoimg[0].smoothScale( int(logoimg[0].width()*0.5),
		int(logoimg[0].height()*0.5) );
	logoimg[3] = logoimg[0].smoothScale( int(logoimg[0].width()*0.25),
		int(logoimg[0].height()*0.25) );
    }
    QCanvasPolygonalItem* i = new ImageItem(logoimg[rand()%4],&canvas);
    i->move(rand()%(canvas.width()-logoimg->width()),
	    rand()%(canvas.height()-logoimg->width()));
    i->setZ(rand()%256+256);
    i->show();
}
*/ 

/*
void Main::addCircle()
{
    QCanvasPolygonalItem* i = new QCanvasEllipse(50,50,&canvas);
    i->setBrush( QColor(rand()%32*8,rand()%32*8,rand()%32*8) );
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addHexagon()
{
    QCanvasPolygon* i = new QCanvasPolygon(&canvas);
    const int size = canvas.width() / 25;
    QPointArray pa(6);
    pa[0] = QPoint(2*size,0);
    pa[1] = QPoint(size,-size*173/100);
    pa[2] = QPoint(-size,-size*173/100);
    pa[3] = QPoint(-2*size,0);
    pa[4] = QPoint(-size,size*173/100);
    pa[5] = QPoint(size,size*173/100);
    i->setPoints(pa);
    i->setBrush( QColor(rand()%32*8,rand()%32*8,rand()%32*8) );
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addPolygon()
{
    QCanvasPolygon* i = new QCanvasPolygon(&canvas);
    const int size = canvas.width()/2;
    QPointArray pa(6);
    pa[0] = QPoint(0,0);
    pa[1] = QPoint(size,size/5);
    pa[2] = QPoint(size*4/5,size);
    pa[3] = QPoint(size/6,size*5/4);
    pa[4] = QPoint(size*3/4,size*3/4);
    pa[5] = QPoint(size*3/4,size/4);
    i->setPoints(pa);
    i->setBrush( QColor(rand()%32*8,rand()%32*8,rand()%32*8) );
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addSpline()
{
    QCanvasSpline* i = new QCanvasSpline(&canvas);
    const int size = canvas.width()/6;
    QPointArray pa(12);
    pa[0] = QPoint(0,0);
    pa[1] = QPoint(size/2,0);
    pa[2] = QPoint(size,size/2);
    pa[3] = QPoint(size,size);
    pa[4] = QPoint(size,size*3/2);
    pa[5] = QPoint(size/2,size*2);
    pa[6] = QPoint(0,size*2);
    pa[7] = QPoint(-size/2,size*2);
    pa[8] = QPoint(size/4,size*3/2);
    pa[9] = QPoint(0,size);
    pa[10]= QPoint(-size/4,size/2);
    pa[11]= QPoint(-size/2,0);
    i->setControlPoints(pa);
    i->setBrush( QColor(rand()%32*8,rand()%32*8,rand()%32*8) );
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addText()
{
    QCanvasText* i = new QCanvasText(&canvas);
    i->setText("QCanvasText");
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addLine()
{
    QCanvasLine* i = new QCanvasLine(&canvas);
    i->setPoints( rand()%canvas.width(), rand()%canvas.height(),
		  rand()%canvas.width(), rand()%canvas.height() );
    i->setPen( QPen(QColor(rand()%32*8,rand()%32*8,rand()%32*8), 6) );
    i->setZ(rand()%256);
    i->show();
}

void Main::addMesh()
{
    int x0 = 0;
    int y0 = 0;

    if ( !tb ) tb = new QBrush( Qt::red );
    if ( !tp ) tp = new QPen( Qt::black );

    int nodecount = 0;

    int w = canvas.width();
    int h = canvas.height();

    const int dist = 30;
    int rows = h / dist;
    int cols = w / dist;

#ifndef QT_NO_PROGRESSDIALOG
    QProgressDialog progress( "Creating mesh...", "Abort", rows,
			      this, "progress", TRUE );
#endif

    QMemArray<NodeItem*> lastRow(cols);
    for ( int j = 0; j < rows; j++ ) {
	int n = j%2 ? cols-1 : cols;
	NodeItem *prev = 0;
	for ( int i = 0; i < n; i++ ) {
	    NodeItem *el = new NodeItem( &canvas );
	    nodecount++;
	    int r = rand();
	    int xrand = r %20;
	    int yrand = (r/20) %20;
	    el->move( xrand + x0 + i*dist + (j%2 ? dist/2 : 0 ),
		      yrand + y0 + j*dist );

	    if ( j > 0 ) {
		if ( i < cols-1 )
		    (new EdgeItem( lastRow[i], el, &canvas ))->show();
		if ( j%2 )
		    (new EdgeItem( lastRow[i+1], el, &canvas ))->show();
		else if ( i > 0 )
		    (new EdgeItem( lastRow[i-1], el, &canvas ))->show();
	    }
	    if ( prev ) {
		(new EdgeItem( prev, el, &canvas ))->show();
	    }
	    if ( i > 0 ) lastRow[i-1] = prev;
	    prev = el;
	    el->show();
	}
	lastRow[n-1]=prev;
#ifndef QT_NO_PROGRESSDIALOG
	progress.setProgress( j );
	if ( progress.wasCancelled() )
	    break;
#endif
    }
#ifndef QT_NO_PROGRESSDIALOG
    progress.setProgress( rows );
#endif
    // qDebug( "%d nodes, %d edges", nodecount, EdgeItem::count() );
}

void Main::addRectangle()
{
    QCanvasPolygonalItem *i = new QCanvasRectangle( rand()%canvas.width(),rand()%canvas.height(),
			    canvas.width()/5,canvas.width()/5,&canvas);
    int z = rand()%256;
    i->setBrush( QColor(z,z,z) );
    i->setPen( QPen(QColor(rand()%32*8,rand()%32*8,rand()%32*8), 6) );
    i->setZ(z);
    i->show();
}*/

void Main::keyPressEvent ( QKeyEvent * e )
{
	
	Car* car= _sel_car;

	switch( e->key() ){
		
	case Qt::Key_Up:
		//if(car && car-> AccLevel() == Car::ACTRL_MODE)  
		if(car && car-> DriverBehavior() == BCTRL_MODE ) 
			car-> setRealVelocity( car->realVelocity()+1);
		break;
	case Qt::Key_Down:
		//if(car && car-> AccLevel() == Car::ACTRL_MODE)  
		if(car && car-> DriverBehavior() == BCTRL_MODE ) 
			car-> setRealVelocity( car->realVelocity()-1);
		break;
	case Qt::Key_Left:
		if(car && car-> DriverBehavior() == BCTRL_MODE) 
		{ 
			car-> setChange2LeftLan( true );
			car-> setChange2RightLan( false );
		}
		break;
	case Qt::Key_Right:
		if(car && car-> DriverBehavior() == BCTRL_MODE ) 
		{
			car-> setChange2RightLan( true );
			car-> setChange2LeftLan( false );
		}
		break;
		
	case Qt::Key_Control:
		if(car) 
		{
			car-> setDriverBehavior(BCTRL_MODE);
			car-> setAccLevel(ACTRL_MODE);
		}
		break;
		
	case Qt::Key_Shift:
		unsigned int a=0;
		double v=0;
		a = (rand()%(3-1+1))+1;
		if(car) 
		{
			v = car-> realVelocity();
			car-> setDriverBehavior(a);

			if(a == AGGRESSIVE)
				car-> setAccLevel(ACC);
			else
			{
				a = (rand()%(2-1+1))+1;
				car-> setAccLevel(a);
			}
			
			//car-> setCRealVelocity(v);
		}
		break;
	}

}

void Main::tryCloseLogFile()
{
	if(_log_file . isOpen() ){
		_log_file . close();
	}
}

void Main::tryOpenLogFile()
{
	_log_file . setName( _fp_log_file );
	if(_log_file . open(IO_WriteOnly) ){
		debug("log file open succes");
	}else
		debug("log file open fault");
}

void Main::tryWriteLogFile()
{
	if( _log_file . isOpen() ){
		QString str = "Time : "+ QString::number(g_timer_ticks/1000.0,'f',2 )
			+ " Network Collision : "+QString::number(g_net_collision)
			+ " Car Collision : "+QString::number(g_car_too_closed) 
			+ " Average Velocity : "+QString::number(coordinator->CountCarVelocity())
			+ "\n"
			;

		static double last_log_time = 0;
		if( (g_timer_ticks - last_log_time) > 1000) //每秒記錄一次 … 應該可以設成變數
		{
			last_log_time = g_timer_ticks;
			_log_file.writeBlock( str.latin1(), str . length () );
		}
	}
}


void Main::slotStop()
{
	/*
	g_timer_ticks = 0;
	
	cMainWindow::slotStop();
	debug("1");
	coordinator->slotStop();
	net_coor->Stop();
	*/
	slotPause();
	tryCloseLogFile();
}


void Main::slotPlay()
{
	cMainWindow::slotPlay();
	debug("2");
	coordinator->slotPlay();
	net_coor->Start();

	pb_center -> setOn( TRUE );

	tryOpenLogFile();
}


void Main::slotPause()
{
	cMainWindow::slotPause();
	debug("3");
	coordinator->slotPause();
	net_coor->Pause();
}

void Main::slotActPosViewer( bool c)
{

	if(c){
		pos_viewer -> slotCarSel( _sel_car );

		pos_viewer -> show();
	}else{
		pos_viewer -> hide();
	}
}

void Main::slotActCenter()
{
	debug("Main::slotActCenter ");

//	slotSetFixedCenter(true);
	actUser->setOn( false );

	ui_state = UI_CENTER_SEL;
}


void Main::slotActUsrSel()
{
	debug("Main::slotActUsrSel");
	actCenter->setOn( false );

	ui_state = UI_USER_SEL;
}

void Main::slotActSimSet()
{
	dlgSimSetting dlg(this);
	dlg . sb_car_render_time -> setValue(g_car_timer_gap);
	dlg . sb_simute_time -> setValue( g_simulate_until);
	dlg . sb_gps_active_time -> setValue(g_gps_update_time);
	dlg . sb_tt_ative_time -> setValue(g_ticks_timer_gap);

	if ( dlg.exec() == QDialog::Accepted ) {
		g_car_timer_gap = dlg . sb_car_render_time -> value();	
		g_simulate_until = dlg . sb_simute_time -> value();
		g_gps_update_time = dlg . sb_gps_active_time -> value();
		g_ticks_timer_gap = dlg . sb_tt_ative_time -> value();
	}
}

void Main::slotSimDone()
{
	net_coor -> slotSimDone();
	QMessageBox::information( this, "Information",
			"Simulation Done." );
}
void Main::slotRandAddDSRCCar()
{
	int a=0;
	bool b_auto_change_lane = false;
	DSRCCar::CHCH_MODE id_polling = DSRCCar::CHCH_ID_POLL;
	bool b_smart_brake = true;
	Initial_Set_dsrc dlg(this);

	//    dlg . setValue( car->realVelocity() , car->maxRealVelocity() );
	//    dlg . le_node_id -> setText( QString::number(car->nodeID()) );

	if ( dlg.exec() == QDialog::Accepted ) {
		gCarAmount = dlg . sp_veh_amount -> value();
		gStartPosition = dlg . sp_veh_start_position -> value();
		gEndPosition = dlg . sp_veh_end_position -> value();

		gMaxVelocity = dlg . sp_max_velocity -> value();
		gMinVelocity = dlg . sp_min_velocity -> value();
		gFrictionFactorNormal = dlg . sp_friction_factor -> value();
		gFrictionFactorEmerg = dlg . sp_friction_factor_em -> value();
		//gSafeDistance = dlg . sp_safe_distance -> value();

		gHumanResponseTime = dlg . sp_human_response_time -> value();
		//gSensorDetectionTime = dlg . sp_sensor_detect_time -> value();
		gSystemHandleTime = dlg . sp_system_handle_time -> value();
		//gMachineWorkTime = dlg . sp_machine_work_time -> value();
		//gTimeSlot = dlg . sp_time_slot -> value();

	//	gTr = (gHumanResponseTime+gSensorDetectionTime+gSystemHandleTime+gMachineWorkTime) * 0.001;
	//	debug("gTr = %f\n", gTr);
	//	debug("gMaxVelocity = %d\n", gMaxVelocity);


		if( dlg . cb_auto_change_lane -> currentItem() == 0 ){
			b_auto_change_lane = true;
		}else {
			b_auto_change_lane = false; 
		}
		if( dlg . cb_ch_selection-> currentItem() == 0 ){
			id_polling = DSRCCar::CHCH_ID_POLL;
		}else{
			id_polling = DSRCCar::CHCH_POLL;
		}
		if( dlg . sb_smart_brake -> currentItem() == 0 ){
			b_smart_brake = true;
		}else{
			b_smart_brake = false;
		}


		resetScrollBar();
		//從這邊加入車子       
		for (a=0; a<gCarAmount; a++) {
			DSRCCar* dc = new DSRCCar(&canvas);
			dc->setZ(rand()%256);
			randCarPos((Car*)dc);
			dc->show();

			coordinator -> add2Coordinator( dc );
			dc -> setNetCoor( net_coor );
			dc -> setAutoChangeLane( b_auto_change_lane );
			dc -> setSmartBrake( b_smart_brake );
			dc -> setChChMode( id_polling );
		}
		coordinator->setFixedCenter(true);
		//coordinator->MoveCarsPosition();
		coordinator -> car_active();
		/* update Scroll Bar 的數值 */
	} 

	mod7 = 70;
}

void Main::slotLogFileSetting()
{
	QString s = QFileDialog::getSaveFileName (
			"./",
			"Log File(*.log)",
			this,
			"open file dialog",
			"Choose a file to save log file" );

	debug("Log File : "+s);
	_fp_log_file = s;
}

void Main::slotFileOpen()
{
	QString conf_fp = QFileDialog::getOpenFileName (
			"./",
			"Configure File(*.conf)",
			this,
			"open file dialog",
			"Choose a file to open configure file" );

	QFile conf_file;
	conf_file . setName( conf_fp );
	if(conf_file . open(IO_ReadOnly) ){
		debug("conf file open succes");
		ParseConfigureFile(conf_file);

		coordinator->setFixedCenter(true);
		coordinator -> car_active();
	}else
		debug("conf file open fault");


	conf_file . close();
}

void Main::ParseConfigureFile(QFile& conf_file)
{
	if ( !conf_file.isOpen() ) 
		return;

	QTextStream stream( &conf_file ); 
	QString pstr;
	unsigned int num;
	while ( !stream.atEnd() ) {
		stream >> pstr;
		pstr = pstr.lower();
		if(pstr == "endposition"){
			stream >> num; 
			gEndPosition = num;
		}else if(pstr == "startposition"){
			stream >> num; 
			gStartPosition = num;
		}else if(pstr == "maxvelocity"){
			stream >> num; 
			gMaxVelocity = num;
		}else if(pstr == "minvelocity"){
			stream >> num; 
			gMinVelocity = num;
		}else if(pstr == "frictionfactornormal"){
			stream >> num; 
			gFrictionFactorNormal = num;
		}else if(pstr == "frictionfactoremerg"){
			stream >> num; 
			gFrictionFactorEmerg = num;
		}else if(pstr == "simulationuntil"){
			stream >> num; 
			g_simulate_until = num;
		}else if(pstr == "caraddbegin"){
			resetScrollBar();
			QString pstr2;
			Car* car = NULL;
			WifiCar* wc = NULL;
			DSRCCar* dc = NULL;
			while(1){
				stream >> pstr2;
				pstr2 = pstr2.lower();

				if(pstr2 == "addwificar" ){
					car = wc = new WifiCar(&canvas);
					randCarPos(car);
					coordinator -> add2Coordinator( wc );
					wc -> setNetCoor( net_coor );
				}else if(pstr2 == "adddsrccar" ){
					car = dc = new DSRCCar(&canvas);
					randCarPos(car);
					coordinator -> add2Coordinator( dc );
					dc -> setNetCoor( net_coor );
				}else if(pstr2 == "autochangelane" ){
					car -> setAutoChangeLane(true);
				}else if( pstr2 == "posx"){
					stream >> num; 
					car -> setRealX( num );
				}else if( pstr2 == "posy"){
					stream >> num; 
					car -> setRealY( num );
				}else if( pstr2 == "lane"){
					stream >> num; 
					car -> setLan( num );
				}else if( pstr2 == "velocity"){
					stream >> num; 
					car -> setRealVelocity(double( num ) );
				}else if( pstr2 == "aggressive"){
					car -> setDriverBehavior( AGGRESSIVE );
				}else if( pstr2 == "conservative"){
					car -> setDriverBehavior( CONSERVATIVE );
				}else if( pstr2 == "normal"){
					car -> setDriverBehavior( NORMAL );
				}else if( pstr2 == "smartbrake"){
					((InternetCar*)car) -> setSmartBrake( true);
				}else if( pstr2 == "caraddend"){
					debug("caraddend");
					break;
				}else {
					debug("[ERR] : Unknow Symbol "+pstr);
				}
			}
			if( car == NULL ){
				debug("[ERR] : Car Add Error");
				return;
			}
			debug("Car-> %f,%f - %f,%f",car->realX(),car->realY(),car->x(),car->y());
			car->show();
		}else {
			debug("[ERR] : Unknow Symbol "+pstr);
		}
	}

	return;
}

void Main :: resetScrollBar()
{
	sc_position->setMaxValue(gStartPosition*1000*10-gEndPosition*1000*10);
	sc_position->setMinValue(0);
}
