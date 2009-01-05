
#include "LeftToolBox.h"

#include <QLabel>

LeftToolBox::LeftToolBox(QWidget *parent):QToolBox(parent) {
	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	setMinimumWidth(200);	// 設定最小值為200
	addItem(new QLabel("second",this),"Road_list");
	addItem(new QLabel("first",this),"Car_list");
	addItem(new QLabel("third",this),"RoadSide_unit");
}

