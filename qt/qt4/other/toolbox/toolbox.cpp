
#include <QTextEdit>

#include "toolbox.h"
#include "mywidget.h"

MyToolBox::MyToolBox(QWidget *parent):QToolBox(parent) {
	// 加入第一個tab - start
	MyWidget *mywidget = new MyWidget(this);
	addItem(mywidget,"first");	// 加入widget到toolbox
	// 加入第一個tab - end
	
	// 加入第二個tab - start
	QTextEdit *textedit = new QTextEdit(this);
	addItem(textedit,"second");	// 加入textedit到toolbox
	// 加入第二個tab - end
}

