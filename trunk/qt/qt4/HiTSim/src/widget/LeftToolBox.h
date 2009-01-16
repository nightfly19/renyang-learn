
#ifndef LEFTTOOLBOX_H
#define LEFTTOOLBOX_H

#include <QToolBox>

class QButtonGroup;

// 此類別主要是配置左邊的快速列,目前主要是建立道路與車子
class LeftToolBox:public QToolBox {
Q_OBJECT
	public:
		LeftToolBox(QWidget *parent=0);
	
	private:
		QWidget *createCarPage();
		QWidget *createButton(const QString &name,QButtonGroup *group=0,const int id=0);
		QWidget *createBaseStationPage();

		QButtonGroup *buttonGroup;
	
	private slots:
		void buttonGroupClicked(int id);
};

#endif // LEFTTOOLBOX_H

