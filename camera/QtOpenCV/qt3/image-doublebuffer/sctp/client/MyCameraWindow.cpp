#include <arpa/inet.h>

#include "MyCameraWindow.h"

MyCameraWindow::MyCameraWindow(QWidget *parent):QWidget(parent)
{
	resize(640,480);
	layout = new QVBoxLayout(this);
	imagelabel = new QLabel(this,"imagelabel");
	imagelabel->setBackgroundMode(Qt::NoBackground);
	layout->addWidget(imagelabel);
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
		return true;
	}
}

void MyCameraWindow::startVideo()
{
	int rd_sz;
	rd_sz = ::sctp_sendmsg(connfd,START_IMAGE,sizeof(START_IMAGE),(struct sockaddr *) NULL,0,0,0,0,0,0);
	if (rd_sz < 0)
		perror("sctp_sendmsg error");
}
