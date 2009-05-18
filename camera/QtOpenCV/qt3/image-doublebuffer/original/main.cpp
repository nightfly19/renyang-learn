#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <assert.h>
#include <qapplication.h>
#include <qwidget.h>

#include "QOpenCVWidget.h"
#include "MyCameraWindow.h"

int main(int argc,char **argv) {
	/* start capturing frames from camera: index = camera_index + domain_offset (CV_CAP_*) */
	// 0     // autodetect
	CvCapture *camera = cvCreateCameraCapture(0);
	// 確定camera必為true, 若為false則會終止程式, 並且印出是在哪一個檔案出現錯誤
	assert(camera);
	// Just a combination of cvGrabFrame and cvRetrieveFrame
	// 也就是取得一個frame啦
	IplImage *image=cvQueryFrame(camera);
	// 確認回傳的frame是沒有問題的
	assert(image);

	// pixel depth in bits
	// 每一個pixel是用幾個bit來表示
	printf("Image depth=%i\n",image->depth);
	// 若是3個channel, 則表示是RGB
	printf("Image nChannels=%i\n",image->nChannels);

	QApplication app(argc,argv);
	MyCameraWindow *mainWin = new MyCameraWindow(camera);
	mainWin->setCaption("OpenCV --> QtImage");
	mainWin->show();
	app.setMainWidget(mainWin);
	int retval = app.exec();

	/* stop capturing/reading and free resources */
	cvReleaseCapture(&camera);
	return retval;
}
