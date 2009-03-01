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
	assert(camera);	// 要確定有東西
	IplImage *image=cvQueryFrame(camera);
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
