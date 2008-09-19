#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qslider.h>
#include <qapplication.h>

#include "mywidget.h"

MyWidget::MyWidget(QWidget *parent,const char *name):QVBox(parent,name)
{
	QPushButton *quit = new QPushButton("Quit",this,"quit");
	quit->setFont(QFont("Times",18,QFont::Bold));

	connect(quit,SIGNAL(clicked()),qApp,SLOT(quit()));

	lcd = new QLCDNumber(4,this,"lcd");

	QSlider *slider = new QSlider(Horizontal,this,"slider");
	slider->setRange(0,99);
	slider->setValue(0);

	QPushButton *hex = new QPushButton("Hex",this,"hex");
	QPushButton *dec = new QPushButton("Dec",this,"dex");
	QPushButton *oct = new QPushButton("Oct",this,"oct");
	QPushButton *bin = new QPushButton("Bin",this,"bin");

	connect(slider,SIGNAL(valueChanged(int)),lcd,SLOT(display(int)));

	connect(hex,SIGNAL(clicked()),this,SLOT(change2hex()));
	connect(dec,SIGNAL(clicked()),this,SLOT(change2dec()));
	connect(oct,SIGNAL(clicked()),this,SLOT(change2oct()));
	connect(bin,SIGNAL(clicked()),this,SLOT(change2bin()));

	connect(lcd,SIGNAL(overflow()),qApp,SLOT(quit()));
}

void MyWidget::change2hex()
{
	lcd->setMode(QLCDNumber::Hex);
}

void MyWidget::change2dec()
{
	lcd->setMode(QLCDNumber::Dec);
}

void MyWidget::change2oct()
{
	lcd->setMode(QLCDNumber::Oct);
}

void MyWidget::change2bin()
{
	lcd->setMode(QLCDNumber::Bin);
}

