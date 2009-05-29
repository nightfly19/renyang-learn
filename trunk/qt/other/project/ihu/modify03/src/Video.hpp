#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <qobject.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "Error.h"

class Video: public QObject
{
	Q_OBJECT
	public:
		Video();
		~Video();
		void end();
		bool initWebcam();
		bool validWebcam();
		bool validFrame(IplImage *);
		void QueryFrame();
		void ScaleFrame();
		void ReleaseImage(IplImage *);
		void ReleaseCamera();
		int imageSize();
		int imageWidth();
		int imageHeight();
		char * imageData();
		void setImageStruct(char *);

	public slots:
		void getScaleImage(char *);

	private:
		// renyang - 用來記錄一開始取得的影像
		IplImage *src;
		// renyang - 用來記錄縮放之後的影像
		IplImage *dst;
		// renyang - 用來記錄camera的設備
		CvCapture *camera;
};

#endif
