#include <qdatetime.h>
#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qlabel.h>
#include <qimage.h>
#include <qprogressdialog.h>
#include "canvas.h"

#include <stdlib.h>

// We use a global variable to save memory - all the brushes and pens in
// the mesh are shared.
static QBrush *tb = 0;
static QPen *tp = 0;

class EdgeItem; // 因為建構子要使用,所以,必需先宣告
class NodeItem; // 因為建構子要使用,所以,必需先宣告

class EdgeItem: public QCanvasLine // 線
{
public:
    EdgeItem( NodeItem*, NodeItem*, QCanvas *canvas );
    void setFromPoint( int x, int y ) ;
    void setToPoint( int x, int y );
    static int count() { return c; }
    void moveBy(double dx, double dy);
private:
    static int c;
};

static const int imageRTTI = 984376;


class ImageItem: public QCanvasRectangle // 圖
{
public:
    ImageItem( QImage img, QCanvas *canvas );
    int rtti () const { return imageRTTI; }
    bool hit( const QPoint&) const;
protected:
    void drawShape( QPainter & );
private:
    QImage image;
    QPixmap pixmap;
};


ImageItem::ImageItem( QImage img, QCanvas *canvas )
    : QCanvasRectangle( canvas ), image(img) // 初始化這兩個物件,一個是父類別,一個是成員
{
    setSize( image.width(), image.height() ); // 設定這一個長方型的大小

#if !defined(Q_WS_QWS)
    pixmap.convertFromImage(image, OrderedAlphaDither); // 轉換圖片到這一個pixmap
#endif
}


void ImageItem::drawShape( QPainter &p ) // 把圖片畫出來
{
// On Qt/Embedded, we can paint a QImage as fast as a QPixmap,
// but on other platforms, we need to use a QPixmap.
#if defined(Q_WS_QWS)
    p.drawImage( int(x()), int(y()), image, 0, 0, -1, -1, OrderedAlphaDither );
#else
    p.drawPixmap( int(x()), int(y()), pixmap );
#endif
}

bool ImageItem::hit( const QPoint &p ) const // 判斷是否有點到物件
{
    int ix = p.x()-int(x());
    int iy = p.y()-int(y());
    if ( !image.valid( ix , iy ) ) // 判斷(ix,iy)是否在image內有效的座標
	return FALSE;
    QRgb pixel = image.pixel( ix, iy ); // 傳回(ix,iy)點的顏色(red,green,blue)組成的成份
    return qAlpha( pixel ) != 0; // 判斷alpha值是否為0,若為0表示滑鼠沒有點到圖片
}

class NodeItem: public QCanvasEllipse // 橢圓
{
public:
    NodeItem( QCanvas *canvas );
    ~NodeItem() {}

    void addInEdge( EdgeItem *edge ) { inList.append( edge ); } // 在列表的最後插入edge
    void addOutEdge( EdgeItem *edge ) { outList.append( edge ); } // 在列表的最後插入edge

    void moveBy(double dx, double dy);

    //    QPoint center() { return boundingRect().center(); }
private:
    QPtrList<EdgeItem> inList; // QPtrList類是一個提供雙向鏈表的模板類
    QPtrList<EdgeItem> outList; // QPtrList類是一個提供雙向鏈表的模板類
};


int EdgeItem::c = 0;


void EdgeItem::moveBy(double, double)
{
    //nothing
}

EdgeItem::EdgeItem( NodeItem *from, NodeItem *to, QCanvas *canvas ) // 初始化建構子
    : QCanvasLine( canvas )
{
    c++; // 增加一個count,表示多了一個邊
    setPen( *tp );
    setBrush( *tb );
    from->addOutEdge( this );
    to->addInEdge( this );
    setPoints( int(from->x()), int(from->y()), int(to->x()), int(to->y()) );
    setZ( 127 );
}

void EdgeItem::setFromPoint( int x, int y ) // 設定起點
{
    setPoints( x,y, endPoint().x(), endPoint().y() );
}

void EdgeItem::setToPoint( int x, int y ) // 設定終點
{
    setPoints( startPoint().x(), startPoint().y(), x, y );
}



void NodeItem::moveBy(double dx, double dy)
{
    QCanvasEllipse::moveBy( dx, dy ); // 使用父類別的moveBy函數

    QPtrListIterator<EdgeItem> it1( inList ); // 設定it1為指到QPtrList的指標變數
    EdgeItem *edge;
    // 若it1指到的位置存放的EdgeItem的指標不是0時，則執行大括號內的指令,直到edge為空指標
    while (( edge = it1.current() )) { 
	++it1;
	edge->setToPoint( int(x()), int(y()) );
    }
    QPtrListIterator<EdgeItem> it2( outList );
    while (( edge = it2.current() )) {
	++it2;
	edge->setFromPoint( int(x()), int(y()) );
    }
}

NodeItem::NodeItem( QCanvas *canvas )
    : QCanvasEllipse( 6, 6, canvas )
{
    setPen( *tp );
    setBrush( *tb );
    setZ( 128 );
}

FigureEditor::FigureEditor(
	QCanvas& c, QWidget* parent,
	const char* name, WFlags f) :
    QCanvasView(&c,parent,name,f)
{
}

void FigureEditor::contentsMousePressEvent(QMouseEvent* e)
{
    QPoint p = inverseWorldMatrix().map(e->pos());
    QCanvasItemList l=canvas()->collisions(p);
    // 注意,it是指到容器的指標,而(*it)是放在容器內的指標
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
	if ( (*it)->rtti() == imageRTTI ) {
	    ImageItem *item= (ImageItem*)(*it); // 轉換回來ImageItem的指標
	    if ( !item->hit( p ) )
		 continue;
	}
	moving = *it; // 設定要移動的物件
	moving_start = p; // 設定起始位置
	return;
    }
    moving = 0; // 沒有要移動的物件
}

void FigureEditor::clear()
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
	if ( *it )
	    delete *it;
    }
}

void FigureEditor::contentsMouseMoveEvent(QMouseEvent* e)
{
    if ( moving ) {
	QPoint p = inverseWorldMatrix().map(e->pos());
	moving->moveBy(p.x() - moving_start.x(),
		       p.y() - moving_start.y()); // 移動相對位置
	moving_start = p; // 重新設定起始位置
	canvas()->update(); // 更新canvas畫面
    }
}



// 初始化動畫
BouncyLogo::BouncyLogo(QCanvas* canvas) :
    QCanvasSprite(0,canvas)
{
    static QCanvasPixmapArray logo("qt-trans.xpm"); // 動畫有多個frame
    setSequence(&logo); // 設定logo
    setAnimated(TRUE); // 動畫可移動
    initPos();
}


const int logo_rtti = 1234;

int BouncyLogo::rtti() const
{
    return logo_rtti;
}

void BouncyLogo::initPos()
{
    initSpeed();
    int trial=1000;
    do {
	move(rand()%canvas()->width(),rand()%canvas()->height());
	advance(0); // 動畫不移動
    } while (trial-- && xVelocity()==0.0 && yVelocity()==0.0); // trial不為0且x,y速度都為0
}

void BouncyLogo::initSpeed()
{
    const double speed = 4.0;
    double d = (double)(rand()%1024) / 1024.0;
    setVelocity( d*speed*2-speed, (1-d)*speed*2-speed );
}

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
	    vx = -vx; // 換反方向
	if ( ny < 0 || ny >= canvas()->height() )
	    vy = -vy; // 換反方向

	for (int bounce=0; bounce<4; bounce++) {
	    QCanvasItemList l=collisions(FALSE); // 沒有那麼精準的傳回所有有碰撞的canvas item
	    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
		QCanvasItem *hit = *it;
		if ( hit->rtti()==logo_rtti && hit->collidesWith(this) ) {
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

static uint mainCount = 0;
static QImage *butterflyimg;
static QImage *logoimg;

Main::Main(QCanvas& c, QWidget* parent, const char* name, WFlags f) :
    QMainWindow(parent,name,f),
    canvas(c)
{
    editor = new FigureEditor(canvas,this); // 繼承canvas view而來
    QMenuBar* menu = menuBar();

    QPopupMenu* file = new QPopupMenu( menu ); // 當parent被刪除,則子類別也會被刪除
    file->insertItem("&Fill canvas", this, SLOT(init()), CTRL+Key_F);
    file->insertItem("&Erase canvas", this, SLOT(clear()), CTRL+Key_E);
    file->insertItem("&New view", this, SLOT(newView()), CTRL+Key_N);
    file->insertSeparator();
    file->insertItem("&Print...", this, SLOT(print()), CTRL+Key_P);
    file->insertSeparator();
    file->insertItem("E&xit", qApp, SLOT(quit()), CTRL+Key_Q);
    menu->insertItem("&File", file); // 建立一個工具列名為File的label

    QPopupMenu* edit = new QPopupMenu( menu );
    edit->insertItem("Add &Circle", this, SLOT(addCircle()), ALT+Key_C);
    edit->insertItem("Add &Hexagon", this, SLOT(addHexagon()), ALT+Key_H);
    edit->insertItem("Add &Polygon", this, SLOT(addPolygon()), ALT+Key_P);
    edit->insertItem("Add Spl&ine", this, SLOT(addSpline()), ALT+Key_I);
    edit->insertItem("Add &Text", this, SLOT(addText()), ALT+Key_T);
    edit->insertItem("Add &Line", this, SLOT(addLine()), ALT+Key_L);
    edit->insertItem("Add &Rectangle", this, SLOT(addRectangle()), ALT+Key_R);
    edit->insertItem("Add &Sprite", this, SLOT(addSprite()), ALT+Key_S);
    edit->insertItem("Create &Mesh", this, SLOT(addMesh()), ALT+Key_M );
    edit->insertItem("Add &Alpha-blended image", this, SLOT(addButterfly()), ALT+Key_A);
    menu->insertItem("&Edit", edit);

    QPopupMenu* view = new QPopupMenu( menu );
    view->insertItem("&Enlarge", this, SLOT(enlarge()), SHIFT+CTRL+Key_Plus);
    view->insertItem("Shr&ink", this, SLOT(shrink()), SHIFT+CTRL+Key_Minus);
    view->insertSeparator();
    view->insertItem("&Rotate clockwise", this, SLOT(rotateClockwise()), CTRL+Key_PageDown);
    view->insertItem("Rotate &counterclockwise", this, SLOT(rotateCounterClockwise()), CTRL+Key_PageUp);
    view->insertItem("&Zoom in", this, SLOT(zoomIn()), CTRL+Key_Plus);
    view->insertItem("Zoom &out", this, SLOT(zoomOut()), CTRL+Key_Minus);
    view->insertItem("Translate left", this, SLOT(moveL()), CTRL+Key_Left);
    view->insertItem("Translate right", this, SLOT(moveR()), CTRL+Key_Right);
    view->insertItem("Translate up", this, SLOT(moveU()), CTRL+Key_Up);
    view->insertItem("Translate down", this, SLOT(moveD()), CTRL+Key_Down);
    view->insertItem("&Mirror", this, SLOT(mirror()), CTRL+Key_Home);
    menu->insertItem("&View", view);

    options = new QPopupMenu( menu );
    dbf_id = options->insertItem("Double buffer", this, SLOT(toggleDoubleBuffer()));
    options->setItemChecked(dbf_id, TRUE);
    menu->insertItem("&Options",options);

    menu->insertSeparator();

    QPopupMenu* help = new QPopupMenu( menu );
    help->insertItem("&About", this, SLOT(help()), Key_F1);
    help->setItemChecked(dbf_id, TRUE); // 很奇怪,當TRUE時,把dbf_id打勾,不懂
    menu->insertItem("&Help",help);

    statusBar();
    // 返回這個窗口的狀態條。如果沒有的話
    // statusBar()會創建一個空的狀態條，並且如果需要也創建一個工具提示組。

    setCentralWidget(editor);

    printer = 0;

    init();
}

void Main::init()
{
    clear();

    static int r=24;
    srand(++r);

    mainCount++;
    butterflyimg = 0; //蝴蝶指標為0
    logoimg = 0; // Qt logo動畫指標為0

    int i;
    for ( i=0; i<canvas.width() / 56; i++) {
	addButterfly();
    }
    for ( i=0; i<canvas.width() / 85; i++) {
	addHexagon();
    }
    for ( i=0; i<canvas.width() / 128; i++) {
	addLogo();
    }
}

Main::~Main()
{
    delete printer;
    if ( !--mainCount ) {
	delete[] butterflyimg; // 刪除指到陣列的指標
	butterflyimg = 0; // 設定指標為0
	delete[] logoimg; // 刪除指到陣列的指標
	logoimg = 0; // 設定指標為0
    }
}

void Main::newView()
{
    // Open a new view... have it delete when closed.
    Main *m = new Main(canvas, 0, 0, WDestructiveClose);
    qApp->setMainWidget(m);
    m->show();
    qApp->setMainWidget(0);
}

void Main::clear()
{
    editor->clear();
}

void Main::help()
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

void Main::aboutQt()
{
    QMessageBox::aboutQt( this, "Qt Canvas Example" );
}

void Main::toggleDoubleBuffer()
{
    bool s = !options->isItemChecked(dbf_id);
    options->setItemChecked(dbf_id,s);
    canvas.setDoubleBuffering(s);
}

void Main::enlarge()
{
    canvas.resize(canvas.width()*4/3, canvas.height()*4/3);
}

void Main::shrink()
{
    canvas.resize(canvas.width()*3/4, canvas.height()*3/4);
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

void Main::zoomIn()
{
    QWMatrix m = editor->worldMatrix();
    m.scale( 2.0, 2.0 );
    editor->setWorldMatrix( m );
}

void Main::zoomOut()
{
    QWMatrix m = editor->worldMatrix();
    m.scale( 0.5, 0.5 );
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


void Main::addSprite()
{
    QCanvasItem* i = new BouncyLogo(&canvas);
    i->setZ(rand()%256);
    i->show();
}

QString butterfly_fn; // 記錄圖片的路徑
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
	logoimg = new QImage[4]; // 產生四種大小的圖片
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
}
