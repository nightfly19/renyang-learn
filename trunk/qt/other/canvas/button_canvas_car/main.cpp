
#include <qapplication.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include "mysource.h"

#include "view.h"
#include "myqcanvas.h"
#include "mybutton.h"

class MyWidget:public QWidget{
public:
	MyWidget(QWidget *parent=0,const char *name=0);
};

MyWidget::MyWidget(QWidget *parent,const char *name):QWidget(parent,name){
	QPushButton *quit = new QPushButton("Quit",this,"quit");
	connect(quit,SIGNAL(clicked()),qApp,SLOT(quit()));

	MyPushButton *put = new MyPushButton(this,"problem");

	MySource *source = new MySource(this,"source");

	MyQCanvas *canvas = new MyQCanvas(0,0);
	View *c = new View(*canvas,this);
	connect (put,SIGNAL(toggled(bool)),c,SLOT(setCanPress(bool)));
	canvas->setAdvancePeriod(30);
	canvas->setDoubleBuffering(true);

	QGridLayout *grid = new QGridLayout(this,2,2,10);

	grid->addWidget(source,0,1,Qt::AlignRight);
	grid->addWidget(quit,0,0);
	grid->addWidget(put,1,0);
	grid->addWidget(c,1,1);
	grid->setColStretch(1,10);
}

int main(int argc,char** argv)
{
	QApplication app(argc,argv);

	MyWidget w(0,0);

	app.setMainWidget(&w);
	w.show();

	return app.exec();
}
