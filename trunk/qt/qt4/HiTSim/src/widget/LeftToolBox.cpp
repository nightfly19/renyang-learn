
#include "LeftToolBox.h"
#include "ToolButton.h"
#include "ToolLabel.h"

#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QPixmap>

LeftToolBox::LeftToolBox(QWidget *parent):QToolBox(parent) {

	addItem(createCarPage(),tr("Car list"));
	addItem(createBaseStationPage(),tr("BaseStation"));
	addItem(createCarLabelPage(),tr("Car list Label"));
}

QWidget *LeftToolBox::createCarPage() {
	

	QGridLayout *carLayout = new QGridLayout;
	buttonGroup = new QButtonGroup(this);
	connect(buttonGroup,SIGNAL(buttonClicked(int)),this,SLOT(buttonGroupClicked(int)));
	buttonGroup->setExclusive(false);	// 必需設定為false,當按扭已按下時,再按一次才會跳起來
	carLayout->addWidget(createButton(":car_b.png",buttonGroup,1),0,0);
	carLayout->addWidget(createButton(":car_g.png",buttonGroup,2),0,1);
	carLayout->addWidget(createButton(":car_r.png",buttonGroup,3),1,0);

	carLayout->setRowStretch(2,10);
	carLayout->setColumnStretch(2,10);

	QWidget *carWidget = new QWidget;
	carWidget->setLayout(carLayout);

	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	setMinimumWidth(200);	// 設定最小值為200

	return carWidget;
}

QWidget *LeftToolBox::createButton(const QString &name,QButtonGroup *group,const int id) {
	
	ToolButton *carButton = new ToolButton;
	carButton->setCheckable(true);
	carButton->setIcon(QIcon(QPixmap(name).scaled(30,30,Qt::KeepAspectRatio)));
	carButton->setIconSize(QSize(50,50));

	if (group != 0){
		group->addButton(carButton, id);
	}

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(carButton,0,0,Qt::AlignHCenter);

	QWidget *widget = new QWidget;
	widget->setLayout(layout);
	return widget;
}

QWidget *LeftToolBox::createCarLabelPage() {
	

	QGridLayout *carLayout = new QGridLayout;
	carLayout->addWidget(createLabel(":car_b.png"),0,0);
	carLayout->addWidget(createLabel(":car_g.png"),0,1);
	carLayout->addWidget(createLabel(":car_r.png"),1,0);

	carLayout->setRowStretch(2,10);
	carLayout->setColumnStretch(2,10);

	QWidget *carWidget = new QWidget;
	carWidget->setLayout(carLayout);

	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	setMinimumWidth(200);	// 設定最小值為200

	return carWidget;
}

QWidget *LeftToolBox::createLabel(const QString &name) {
	
	ToolLabel *carLabel = new ToolLabel;
	carLabel->setPixmap(QPixmap(name));

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(carLabel,0,0,Qt::AlignHCenter);

	QWidget *widget = new QWidget;
	widget->setLayout(layout);
	return widget;
}

void LeftToolBox::buttonGroupClicked(int id) {
	QList<QAbstractButton *> buttons = buttonGroup->buttons();
	foreach (QAbstractButton *button,buttons) {
		if (buttonGroup->button(id) != button)
			button->setChecked(false);
	}
}

QWidget *LeftToolBox::createBaseStationPage() {
	

	QGridLayout *carLayout = new QGridLayout;
	carLayout->addWidget(createButton(":antenna.png"),0,0);

	carLayout->setRowStretch(2,10);
	carLayout->setColumnStretch(2,10);

	QWidget *carWidget = new QWidget;
	carWidget->setLayout(carLayout);

	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	setMinimumWidth(200);	// 設定最小值為200

	return carWidget;
}
