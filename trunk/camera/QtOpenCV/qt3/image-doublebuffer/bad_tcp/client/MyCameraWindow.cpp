
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
	client_socket = new ClientSocket("127.0.0.1",30000);
	// sn = new QSocketNotifier(client_socket->get_connfd(),QSocketNotifier::Read,this);
	// QObject::connect(sn,SIGNAL(activated(int)),this,SLOT(Recvdata()));
}

MyCameraWindow::~MyCameraWindow()
{
	// do nothing
}

// 接收到struct,顯示在QLabel中
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

void MyCameraWindow::Recvdata()
{
	/*
	char filebuffer[MAXRECV];
	memset(filebuffer,0,MAXRECV);
	int Readbyte=0;
	Readbyte=*client_socket>>filebuffer;
	if (strncmp(filebuffer,"#over#",strlen("#over#"))==0)
		printf("byt!!\n");*/
	// if (sn) delete sn;
	//sn = new QSocketNotifier(client_socket->get_connfd(),QSocketNotifier::Read,this);
	//QObject::connect(sn,SIGNAL(activated(int)),this,SLOT(Recvdata()));
	//printf("x");
}
