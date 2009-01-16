
#include "CentralWidget.h"
#include "LeftToolBox.h"

#include <QHBoxLayout>

CentralWidget::CentralWidget(QWidget *parent):QWidget(parent) {
	setAcceptDrops(true);
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(new LeftToolBox(this));
	view =  new MainView(this);
	layout->addWidget(view);
	setLayout(layout);
}

void CentralWidget::addCar() {
	view->addCar();
}

