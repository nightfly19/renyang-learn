
#include <QTextEdit>

#include "toolbox.h"
#include "mywidget.h"

MyToolBox::MyToolBox(QWidget *parent):QToolBox(parent) {

	// 建立要加入的物件 - start
	MyWidget *mywidget = new MyWidget(this);
	QTextEdit *textedit = new QTextEdit(this);
	// 建立要如入的物件 - end

	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	setMinimumWidth(mywidget->sizeHint().width());
	// 加入第一個tab - start
	addItem(mywidget,"first");	// 加入widget到toolbox
	// 加入第一個tab - end
	
	// 加入第二個tab - start
	addItem(textedit,"second");	// 加入textedit到toolbox
	// 加入第二個tab - end
}

