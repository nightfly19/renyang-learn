
#include <QVBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>

#include "mywidget.h"

MyWidget::MyWidget(QWidget *parent):QWidget(parent) {
	QVBoxLayout *layout = new QVBoxLayout(this);
	QPixmap *pixmap = 0;
	QIcon icon(":images/car_b.png");
	// 建立一個QToolButton - start
	QToolButton *button = new QToolButton(this);
	button->setIcon(icon);
	button->setIconSize(QSize(2*button->iconSize().width(),2*button->iconSize().height()));	// 把圖放大兩倍
	layout->addWidget(button);
	// 建立一個QToolButton - end
	
	// 建立一個QPushButton - start
	pixmap = new QPixmap(":images/car_g.png");
	QPushButton *pushbutton = new QPushButton(*pixmap,0,this);
	layout->addWidget(pushbutton);
	// 建立一個QPushButton - end
	
	// 建立一個QLabel - start
	QLabel *label = new QLabel(this);
	pixmap = new QPixmap(":images/car_r.png");
	label->setPixmap(*pixmap);
	layout->addWidget(label);
	// 建立一個QLabel - end
	setLayout(layout);
}


