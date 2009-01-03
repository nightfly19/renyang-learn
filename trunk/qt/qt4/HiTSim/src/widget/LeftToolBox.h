
#ifndef LEFTTOOLBOX_H
#define LEFTTOOLBOX_H

#include <QToolBox>

// 此類別主要是配置左邊的快速列,目前主要是建立道路與車子
class LeftToolBox:public QToolBox {
Q_OBJECT
	public:
		LeftToolBox(QWidget *parent=0);
};

#endif // LEFTTOOLBOX_H

