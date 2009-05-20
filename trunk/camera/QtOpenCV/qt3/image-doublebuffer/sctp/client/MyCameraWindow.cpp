#include <arpa/inet.h>

#include "MyCameraWindow.h"

MyCameraWindow::MyCameraWindow(QWidget *parent):QWidget(parent)
{
	resize(640,480);
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
	char buffer[MAX_BUFFER];
	int ret;
	bzero(buffer,sizeof(buffer));
	ret = ::sctp_recvmsg(connfd,buffer,MAX_BUFFER,(struct sockaddr *) NULL,(socklen_t *)NULL,NULL,NULL);
	if (ret <=0)
		return ;
	else
	{
		if (strcmp(buffer,IMAGE_OK)==0)
			ret = ::sctp_sendmsg(connfd,IMAGE_SPACE_PREPARE_OK,strlen(IMAGE_SPACE_PREPARE_OK),NULL,0,0,0,0,0,0);
		else if (strcmp(buffer,IMAGE_END)==0)
		{
			// 把圖片放到qlabel
			begin = 0;
			printf("I got a image\n");
		}
		else
		{
			memcpy(((char *)&matrix)+begin,buffer,ret);
			begin+=ret;
		}
	}
}
