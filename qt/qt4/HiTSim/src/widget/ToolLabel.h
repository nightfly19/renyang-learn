
#ifndef TOOLLABEL_H
#define TOOLLABEL_H

#include <QLabel>

class ToolLabel:public QLabel {
	public:
		ToolLabel(QWidget *parent=0);

	protected:
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

	private:
		void execDrag();
};

#endif // TOOLLABEL_H

