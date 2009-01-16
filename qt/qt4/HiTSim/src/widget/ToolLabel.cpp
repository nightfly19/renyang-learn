
#include "ToolLabel.h"

#include <QMimeData>
#include <QDrag>

ToolLabel::ToolLabel(QWidget *parent):QLabel(parent) {
	// do nothing
}

void ToolLabel::mousePressEvent(QMouseEvent *event) {
	QLabel::mousePressEvent(event);
}

void ToolLabel::mouseMoveEvent(QMouseEvent *event) {
	execDrag();
	QLabel::mouseMoveEvent(event);
}

void ToolLabel::execDrag() {
	QMimeData *mimeData = new QMimeData;
	mimeData->setText("button");

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);

	drag->exec();
}
