
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
		QWidget *createCarButton(const QString &name,QButtonGroup *group,const int id);

		QButtonGroup *buttonGroup;
};

#endif // LEFTTOOLBOX_H

