#include <arpa/inet.h>

#include "MyCameraWindow.h"

MyCameraWindow::MyCameraWindow(QWidget *parent):QWidget(parent)
{
	resize(640,480);
	QImage temp(640,480,32);
	image = temp;
	layout = new QVBoxLayout(this);
	imagelabel = new QLabel(this,"imagelabel");
	imagelabel->setBackgroundMode(Qt::NoBackground);
	layout->addWidget(imagelabel);
	begin=0;
	notifier = NULL;
}

bool MyCameraWindow::Connect2Host(QString host, int port)
{
	int ret;

	// Create an SCTP TCP-Style Socket
	connfd = ::socket(AF_INET,SOCK_STREAM,IPPROTO_SCTP);
	if (connfd == -1) {
		perror("socket error");
		return false;
	}
	
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(host.latin1());

	ret = ::connect(connfd,(struct sockaddr *) &servaddr,sizeof(servaddr));
	if (connfd == -1) {
		perror("connect error");
		return false;
	}
	else
	{
		int flag=1;
		ret = ::setsockopt(connfd,IPPROTO_SCTP,SCTP_NODELAY,&flag,sizeof(flag));
		if (ret==-1) {
			perror("set SCTP_NODELAY error");
		}
		notifier = new QSocketNotifier(connfd,QSocketNotifier::Read,this);
		connect(notifier,SIGNAL(activated(int)),this,SLOT(Recvdata()));
		return true;
	}
}

void MyCameraWindow::startVideo()
{
	int rd_sz;
	rd_sz = ::sctp_sendmsg(connfd,IMAGE_START,strlen(IMAGE_START),(struct sockaddr *) NULL,0,0,0,0,0,0);
	if (rd_sz < 0)
		perror("sctp_sendmsg error");
}

void MyCameraWindow::Recvdata()
{
	printf("got some data\n");
	char buffer[MAX_BUFFER];
	int ret;
	bzero(buffer,sizeof(buffer));
	ret = ::sctp_recvmsg(connfd,buffer,MAX_BUFFER,(struct sockaddr *) NULL,(socklen_t *)NULL,NULL,NULL);
	if (ret <=0)
	{
		printf("sctp_recvmsg error\n");
		return ;
	}
	else
	{
		printf("get some data\n");
		if (strcmp(buffer,IMAGE_OK)==0)
			ret = ::sctp_sendmsg(connfd,IMAGE_SPACE_PREPARE_OK,strlen(IMAGE_SPACE_PREPARE_OK),NULL,0,0,0,0,0,0);
		else if (strcmp(buffer,IMAGE_END)==0)
		{
			// 把圖片放到qlabel
			printf("total:%d\n",begin);
			begin = 0;
			printf("I got a image\n");
			setCameraImage();
		}
		else
		{
			// 把接收到的檔案收到struct中
			char *ptr=(char *)&matrix;
			memcpy(ptr+begin,buffer,ret);
			begin+=ret;
			ret = ::sctp_sendmsg(connfd,IMAGE_RECVING,strlen(IMAGE_RECVING),NULL,0,0,0,0,0,0);
		}
	}
}

void MyCameraWindow::setCameraImage()
{
	QPixmap pix(matrix.width,matrix.height);
	printf("height:%d\nwidth:%d\n",matrix.height,matrix.width);
	if (matrix.height != image.height() || matrix.width != image.width())
	{
		QImage temp(matrix.width,matrix.height,32);
		image = temp;
	}
	for (int y=0;y<matrix.height;y++)
	{
		for (int x=0;x<matrix.width;x++)
		{
			image.setPixel(x,y,qRgb(matrix.data[3*(x+y*matrix.width)+2],matrix.data[3*(x+y*matrix.width)+1],matrix.data[3*(x+y*matrix.width)]));
		}
	}
	pix.convertFromImage(image);
	imagelabel->setPixmap(pix);
}

void MyCameraWindow::start()
{
	// 每XX ms執行timerEvent()一次
	startTimer(33);
}

void MyCameraWindow::timerEvent(QTimerEvent*)
{
	startVideo();
}
