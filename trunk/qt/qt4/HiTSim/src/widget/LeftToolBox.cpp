
#include "LeftToolBox.h"

#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QToolButton>

LeftToolBox::LeftToolBox(QWidget *parent):QToolBox(parent) {

	addItem(createCarPage(),tr("Car list"));
}

QWidget *LeftToolBox::createCarPage() {
	

	QGridLayout *carLayout = new QGridLayout;
	buttonGroup = new QButtonGroup(this);
	buttonGroup->setExclusive(true);
	carLayout->addWidget(createCarButton(":car_b.png",buttonGroup,1),0,0);
	carLayout->addWidget(createCarButton(":car_g.png",buttonGroup,1),0,1);
	carLayout->addWidget(createCarButton(":car_r.png",buttonGroup,1),1,0);

	carLayout->setRowStretch(2,10);
	carLayout->setColumnStretch(2,10);

	QWidget *carWidget = new QWidget;
	carWidget->setLayout(carLayout);

	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	setMinimumWidth(200);	// 設定最小值為200

	return carWidget;
}

QWidget *LeftToolBox::createCarButton(const QString &name,QButtonGroup *group,const int id) {
	
	QToolButton *carButton = new QToolButton;
	carButton->setCheckable(true);
	carButton->setIcon(QIcon(QPixmap(name).scaled(30,30,Qt::KeepAspectRatio)));
	carButton->setIconSize(QSize(50,50));

	group->addButton(carButton, id);

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(carButton,0,0,Qt::AlignHCenter);

	QWidget *widget = new QWidget;
	widget->setLayout(layout);
	return widget;
}

