#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <assert.h>
#include <qapplication.h>
#include <qwidget.h>

#include "QOpenCVWidget.h"
#include "MyCameraWindow.h"

int main(int argc,char **argv) {
	CvCapture *camera = cvCreateCameraCapture(0);
	// 確定camera必為true, 若為false則會終止程式, 並且印出是在哪一個檔案出現錯誤
	assert(camera);
	// 取得一個frame
	IplImage *image=cvQueryFrame(camera);
	// 確認回傳的frame是沒有問題的
	assert(image);

	printf("Image depth=%i\n",image->depth);
	printf("Image nChannels=%i\n",image->nChannels);

	QApplication app(argc,argv);
	MyCameraWindow *mainWin = new MyCameraWindow(camera);
	mainWin->setCaption("OpenCV --> QtImage");
	mainWin->show();
	app.setMainWidget(mainWin);
	int retval = app.exec();

	cvReleaseCapture(&camera);
	return retval;
}
