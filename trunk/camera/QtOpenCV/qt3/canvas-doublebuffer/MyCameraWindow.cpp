
#include "MyCameraWindow.h"

MyCameraWindow::MyCameraWindow(CvCapture *cam,QWidget *parent):QWidget(parent) {
	camera = cam;
	QVBoxLayout *layout = new QVBoxLayout(this);
	cvwidget = new QOpenCVWidget(this);
	layout->addWidget(cvwidget);
	resize(500,400);

	startTimer(100);

}

void MyCameraWindow::timerEvent(QTimerEvent*) {
	IplImage *image=cvQueryFrame(camera);
	cvwidget->putImage(image);
}
