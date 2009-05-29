#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <qobject.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

class Video: public QObject
{
	Q_OBJECT
	public:
		Video();
};

#endif
