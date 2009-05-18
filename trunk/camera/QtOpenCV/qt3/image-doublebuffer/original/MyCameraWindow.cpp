
#include "MyCameraWindow.h"

MyCameraWindow::MyCameraWindow(CvCapture *cam,QWidget *parent):QWidget(parent) {
	camera = cam;
	QVBoxLayout *layout = new QVBoxLayout(this);
	cvwidget = new QOpenCVWidget(this);
	layout->addWidget(cvwidget);
	resize(500,400);

	// 每100ms執行timerEvent()
	startTimer(100);

}

void MyCameraWindow::timerEvent(QTimerEvent*) {
	// 讀取一個frame
	IplImage *image=cvQueryFrame(camera);
	// 把讀取到的frame送到cvwidget顯示
	cvwidget->putImage(image);
}
