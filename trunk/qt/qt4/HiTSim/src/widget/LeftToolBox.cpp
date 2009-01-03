
#include "LeftToolBox.h"

#include <QLabel>

LeftToolBox::LeftToolBox(QWidget *parent):QToolBox(parent) {
	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	addItem(new QLabel("first",this),"Car_list");
	addItem(new QLabel("second",this),"Road_list");
}

