#include <qapplication.h>
#include <qwidget.h>
#include <qlabel.h>

class EventLabel:public QLabel {
	public:
		EventLabel(QWidget *parent,const char *name=0);
	
	protected:
		void mousePressEvent(QMouseEvent *e);
		void mouseMoveEvent(QMouseEvent *e);
};

EventLabel::EventLabel(QWidget *parent,const char *name):QLabel(parent,name){
	// do nothing
}

void EventLabel::mousePressEvent(QMouseEvent *e){
	QString msg;
	msg.sprintf("(%d,%d)",e->x(),e->y());
	this->setText(msg);
	setMouseTracking(true); //打開滑鼠追蹤,這樣mouseMoveEvent才能在不用按滑鼠時,就有作用
}

void EventLabel::mouseMoveEvent(QMouseEvent *e){
	QString msg;
	msg.sprintf("(%d,%d)",e->x(),e->y());
	this->setText(msg);
}

int main(int argc,char **argv){

	QApplication app(argc,argv);
	
	EventLabel hello(0);
	hello.resize(1000,300);
	app.setMainWidget(&hello);
	hello.show();

	return app.exec();
}
