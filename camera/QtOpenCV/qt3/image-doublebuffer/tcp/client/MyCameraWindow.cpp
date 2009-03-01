
#include "MyCameraWindow.h"

MyCameraWindow::MyCameraWindow(QWidget *parent):QWidget(parent)
{
	resize(640,480);
	QImage temp(640,480,32);
	image = temp;
	layout = new QVBoxLayout(this);
	imagelabel = new QLabel(this,"imagelabel");
	layout->addWidget(imagelabel);
	// 以下三行是測試透過imagedata是否是否有效果
	// struct imagedata pixel;
	// memset(&pixel,0,sizeof(struct imagedata));
	// setCameraImage(pixel);
}

MyCameraWindow::~MyCameraWindow()
{
	// do nothing
}

void MyCameraWindow::setCameraImage(struct imagedata &structimage)
{
	for (int y=0;y<480;y++)
	{
		for (int x=0;x<640;x++)
		{
			image.setPixel(x,y,qRgb(structimage.r_pixel[y][x],structimage.g_pixel[y][x],structimage.b_pixel[y][x]));
		}
	}
	pix.convertFromImage(image);
	imagelabel->setPixmap(pix);
}
