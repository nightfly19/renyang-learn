#ifndef MYCAMERAWINDOW_H
#define MYCAMERAWINDOW_H

#include <qwidget.h>
#include <qlayout.h>
#include "QOpenCVWidget.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>

class MyCameraWindow:public QWidget
{
	Q_OBJECT
	private:
		QOpenCVWidget *cvwidget;
		CvCapture *camera;
	public:
		MyCameraWindow(CvCapture *cam,QWidget *parent=0);

	protected:
		void timerEvent(QTimerEvent*);
};

#endif
