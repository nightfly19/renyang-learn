
#include "ToolButton.h"

#include <QMimeData>
#include <QDrag>

ToolButton::ToolButton(QWidget *parent):QToolButton(parent) {
	setAcceptDrops(true);
}

void ToolButton::mousePressEvent(QMouseEvent *event) {
	QToolButton::mousePressEvent(event);
}

void ToolButton::mouseMoveEvent(QMouseEvent *event) {
	execDrag();
	QToolButton::mouseMoveEvent(event);
}

void ToolButton::execDrag() {
	QMimeData *mimeData = new QMimeData;
	mimeData->setText("button");

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);

	drag->exec();
}
