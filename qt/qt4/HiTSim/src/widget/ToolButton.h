
#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QToolButton>

class ToolButton:public QToolButton {
	public:
		ToolButton(QWidget *parent=0);

	protected:
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
	private:
		void execDrag();
};

#endif // TOOLBUTTON_H

