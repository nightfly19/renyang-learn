
#ifndef MYSOURCE_H
#define MYSOURCE_H

#include <qlabel.h>

class MySource:public QLabel{
public:
	MySource(QWidget *parent=0,const char *name=0);
protected:
	void dragEnterEvent(QDragEnterEvent *e);
	void dragLeaveEvent(QDragLeaveEvent *);
	void mousePressEvent(QMouseEvent *e);
};

#endif // MYSOURCE_H

