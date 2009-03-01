
#include "MyCameraWindow.h"

MyCameraWindow::MyCameraWindow(QWidget *parent):QWidget(parent)
{
	resize(640,480);
	pix.resize(640,480);
	layout = new QVBoxLayout(this);
	imagelabel = new QLabel(0,"imagelabel");
	layout->addWidget(imagelabel);
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
