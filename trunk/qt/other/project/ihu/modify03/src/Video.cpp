#include "Video.hpp"

Video::Video()
{
#ifdef REN_DEBUG
	qWarning("Video::Video()");
#endif
	src = NULL;
	dst = NULL;
	camera = NULL;
}

bool Video::initWebcam()
{
#ifdef REN_DEBUG
	qWarning("Video::initWebcam()");
#endif
	camera = cvCreateCameraCapture(0);
	if (camera == NULL)
	{
		return false;
	}
	return true;
}

bool Video::validWebcam()
{
#ifdef REN_DEBUG
	qWarning("Video::validWebcam()");
#endif
	if (camera == NULL)
		return false;
	return true;
}

Video::~Video()
{
#ifdef REN_DEBUG
	qWarning("Video::~Video()");
#endif
	end();
}

void Video::end()
{
#ifdef REN_DEBUG
	qWarning("Video::end()");
#endif
	ReleaseCamera();
	// ReleaseImage(src);
	// ReleaseImage(dst);
}

void Video::QueryFrame()
{
#ifdef REN_DEBUG
	qWarning("Video::QueryFrame()");
#endif
	if (validWebcam())
	{
		src = cvQueryFrame(camera);
		if (src == NULL)
		{
			throw Error(QString("QueryFrame Error"));
		}
	}
	else
	{
		throw Error(QString("The camera is invalid"));
	}
}

bool Video::validFrame(IplImage *target)
{
#ifdef REN_DEBUG
	qWarning("Video::validFrame()");
#endif
	if (target == NULL)
	{
		return false;
	}
	return true;
}

void Video::ScaleFrame()
{
#ifdef REN_DEBUG
	qWarning(QString("Video::ScaleFrame()"));
#endif
	if (validFrame(src))
	{
		// renyang - 先釋放之前使用的空間
		// ReleaseImage(dst);
		CvSize dst_cvsize;
		// renyang - 固定縮放為160*120
		dst_cvsize.width = 160;
		dst_cvsize.height = 120;
		dst = cvCreateImage(dst_cvsize,src->depth,src->nChannels);
		if (dst == NULL)
		{
			throw Error(QString("cvCreateImage Error"));
		}
		cvResize(src,dst,CV_INTER_LINEAR);
		// renyang-modify - 釋放source data
		// ReleaseImage(src);
	}
}

void Video::ReleaseImage(IplImage *target)
{
#ifdef REN_DEBUG
	qWarning("Video::ReleaseImage()");
#endif
	if (validFrame(target))
	{
		cvReleaseImage(&target);
		target = NULL;
	}
}

void Video::ReleaseCamera()
{
#ifdef REN_DEBUG
	qWarning("Video::ReleaseCamera()");
#endif
	if (validWebcam())
	{
		cvReleaseCapture(&camera);
		camera = NULL;
	}
}

int Video::imageSize()
{
#ifdef REN_DEBUG
	qWarning("Video::imageSize()");
#endif
	if (validFrame(dst))
	{
		return dst->imageSize;
	}
	else
	{
		qWarning("get the imageSize error");
		return -1;
	}
}

char *Video::imageData()
{
#ifdef REN_DEBUG
	qWarning("Video::imageData()");
#endif
	if (validFrame(dst))
	{
		return dst->imageData;
	}
	else
	{
		qWarning("get the image data error");
		return NULL;
	}
}

int Video::imageWidth()
{
#ifdef REN_DEBUG
	qWarning("Video::imageWidth()");
#endif
	if (validFrame(dst))
	{
		return dst->width;
	}
	else
	{
		qWarning("get the dst width error");
		return -1;
	}
}

int Video::imageHeight()
{
#ifdef REN_DEBUG
	qWarning("Video::imageHeight()");
#endif
	if (validFrame(dst))
	{
		return dst->height;
	}
	else
	{
		qWarning("get the dst height error");
		return -1;
	}
}

// renyang-modify - 由Phone呼叫, 把放下來的要送出去的資料結構塞滿要送出去的資料
void Video::setImageStruct(char *image_matrix)
{
#ifdef REN_DEBUG
	qWarning("Video::setImageStruct()");
#endif
	int *temp;
	if (validFrame(dst))
	{
		temp = (int *)image_matrix;
		*temp = imageWidth();
		*(temp+1) = imageHeight();
		memcpy(temp+2,imageData(),imageSize());
	}
}

// renyang-modify - 把取得的本地端影像放到image_matrix這一個位址中
void Video::getScaleImage(char *image_matrix)
{
#ifdef REN_DEBUG
	qWarning("Video::getScaleImage()");
#endif
	// renyang-modify - 取得一個frame
	QueryFrame();
	// renyang-modify - 縮放到固定的大小
	ScaleFrame();
	// renyang-modify - 把縮放的大小放到image_matrix中準備送出去
	setImageStruct(image_matrix);
}
