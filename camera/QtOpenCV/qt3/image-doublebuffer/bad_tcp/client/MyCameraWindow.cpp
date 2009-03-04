
#include "MyCameraWindow.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "transmit.h"

MyCameraWindow::MyCameraWindow(QWidget *parent):QWidget(parent),packageindex(0)
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
	sn = new QSocketNotifier(client_socket->get_connfd(),QSocketNotifier::Read,this);
	QObject::connect(sn,SIGNAL(activated(int)),this,SLOT(Recvdata()));
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
	if (sn) delete sn;	// 若不刪掉的話,會一直觸發這一個function,因為,還有資料沒有接收到
	char filebuffer[MAXRECV];
	memset(filebuffer,0,MAXRECV);
	struct imagedata image;
	memset(&image,0,sizeof(struct imagedata));
	char *char_image = (char*) &image;
	int Readbyte=0;
	/*
	Readbyte=*client_socket>>filebuffer;
	while (strncmp(filebuffer,"#over#",strlen("#over#"))!=0)
	{
		Readbyte = *client_socket>>filebuffer;
		// strcpy(char_image+MAXRECV*packageindex,filebuffer);
		memcpy(char_image+packageindex,filebuffer,MAXRECV);
		packageindex += MAXRECV;
	}*/
		// printf("%d\n",packageindex);
	RecvStruct(client_socket,char_image);
		printf("%c\n",image.b_pixel[479][639]);
		printf("byt!!\n");
		packageindex=0;
		FILE *fp;
		fp = fopen("after","wb");
		fwrite(&image,1,sizeof(struct imagedata),fp);
		fclose(fp);
}
