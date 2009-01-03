#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>

#include "MainView.h"

// 此類別主要是用來配置在MainWindow的central widget內的元件
// 主要功能是layout
class CentralWidget:public QWidget {
Q_OBJECT
	public:
		CentralWidget(QWidget *parent=0);
	
	public slots:
		void addCar();
	private:
		MainView *view;
};

#endif // CENTRALWIDGET_H

