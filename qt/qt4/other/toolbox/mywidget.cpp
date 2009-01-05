
#include <QGridLayout>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>

#include "mywidget.h"

MyWidget::MyWidget(QWidget *parent):QWidget(parent) {
	QGridLayout *layout = new QGridLayout(this);
	QPixmap *pixmap = 0;
	QIcon icon(":images/car_b.png");
	// 建立一個QToolButton - start
	QToolButton *button = new QToolButton(this);
	button->setIcon(icon);
	button->setIconSize(QSize(3*button->iconSize().width(),3*button->iconSize().height()));	// 把圖放大兩倍
	layout->addWidget(button,0,0);
	// 建立一個QToolButton - end
	
	// 建立一個QPushButton - start
	pixmap = new QPixmap(":images/car_g.png");
	QPushButton *pushbutton = new QPushButton(*pixmap,0,this);
	layout->addWidget(pushbutton,0,1);
	// 建立一個QPushButton - end
	
	// 建立一個QLabel - start
	QLabel *label = new QLabel(this);
	pixmap = new QPixmap(":images/car_r.png");
	label->setPixmap(*pixmap);
	layout->addWidget(label,1,1);
	// 建立一個QLabel - end
	setLayout(layout);
}


