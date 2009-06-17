
#include "SctpSocketServer.h"

SctpSocketServer::SctpSocketServer(Q_UINT16 port, int backlog,QObject *parent,const char *name):QObject(parent,name)
{
	begin = 0;

	// 一開始就建立一個等待對方連線過來的socket
	sctp_newconnection_notification = NULL;
	sctp_notification = NULL;
	camera = NULL;
	image = NULL;
	ready_image = NULL;

	listenfd = ::socket(AF_INET,SOCK_STREAM,IPPROTO_SCTP);
	if (listenfd == -1) {
		perror("Error opening socket");
		exit(-1);
	}
	::bzero(&serv_addr,sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if ((::bind(listenfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)))==-1) {
		perror("Bind Error");
		exit(-1);
	}

	// listen - listen for socket connections and limit the queue of incoming connections
	if (::listen(listenfd,backlog)==-1) {
		perror("Listen error");
		exit(-1);
	}

	sctp_newconnection_notification = new QSocketNotifier(listenfd,QSocketNotifier::Read,this,"sctp_newconnection_notification");
	connect(sctp_newconnection_notification,SIGNAL(activated(int)),this,SLOT(SCTPServerAccept(int)));
	
	// 顯示等待連接啦
	qWarning("Waiting connection ...");
}

int SctpSocketServer::SCTPServerAccept(int s)
{
	struct sockaddr_in cli_addr;
	socklen_t client_len;
	client_len = sizeof(cli_addr);
	int ret;
	connfd = ::accept(s,(struct sockaddr *) &cli_addr,&client_len);
	if (connfd == -1) {
		perror("accept error");
		exit(-1);
	}
	
	int flag = 1;
	ret = ::setsockopt(connfd,IPPROTO_SCTP,SCTP_NODELAY,&flag,sizeof(flag));
	if (ret == -1)
	{
		qWarning("set SCTP_NODELAY error");
	}

	sctp_notification = new QSocketNotifier(connfd,QSocketNotifier::Read,this,"sctp_notification");
	connect(sctp_notification,SIGNAL(activated(int)),this,SLOT(recv(int)));
	qWarning("accept success!!");
	qWarning("the connfd is %d",connfd);
	return connfd;
}

int SctpSocketServer::recv(int s)
{
	int ret;
	char recvbuffer[1024];
	struct sctp_sndrcvinfo sinfo;
	int msg_flags;
	struct sockaddr_in peer;
	socklen_t peerlen = sizeof(peer);

	bzero(recvbuffer,1024);
	ret = ::sctp_recvmsg(s,recvbuffer,1024,(struct sockaddr *) &peer,&peerlen,&sinfo,&msg_flags);
	if (ret == -1) {
		perror("sctp_recvmsg error");
		exit(-1);
	}
	else if (ret == 0) {
		delete sctp_notification;
		sctp_notification = NULL;
		close(s);
		qWarning("clinet close");
		return ret;
	}

	if (msg_flags & MSG_NOTIFICATION)
	{
		qWarning("there is some notification");
		return ret;
	}

	analyze(recvbuffer);

	connfd = s;
	return ret;
}

void SctpSocketServer::analyze(char *instruction)
{
	int ret;
	if (strcmp(instruction,IMAGE_START)==0)
	{
		if (camera == NULL) {
			// 設定要使用的webcam設備
			camera = cvCreateCameraCapture(0);
			if (camera == NULL)
			{
				// 通知client端取得frame過程錯誤
				ret = ::sctp_sendmsg(connfd,IMAGE_ERROR,strlen(IMAGE_ERROR),(struct sockaddr *) NULL,0,0,0,0,0,0);
			}
		}

		// 嘗試取得第一個frame
		if ((image=cvQueryFrame(camera))==NULL)
		{
			// 通知client取得frame過程錯誤
			ret = sctp_sendmsg(connfd,IMAGE_ERROR,strlen(IMAGE_ERROR),(struct sockaddr *) NULL,0,0,0,0,0,0);
		}
		else
		{
			// 通知client可以正常取得frame
			ret = sctp_sendmsg(connfd,IMAGE_OK,strlen(IMAGE_OK),(struct sockaddr *) NULL,0,0,0,0,0,0);
		}
	}
	else if (strcmp(instruction,IMAGE_SPACE_PREPARE_OK)==0)
	{
		// 對方要求開始送圖片, 初始化begin
		begin = 0;

		// client端表示它的buffer空間準備好啦
		qWarning("the image buffer prepare ok");
		// 縮放Image
		Scale(scale_value);

		// 記算一個圖片要多少大小
		totalsize = 3*(ready_image->height)*(ready_image->width)+8;

		// 填入要傳送的資料
		matrix.height = ready_image->height;
		matrix.width = ready_image->width;
		memcpy(matrix.data,ready_image->imageData,3*matrix.height*matrix.width);
		// 傳送結構啦
		SDStruct();
		// cvReleaseImage(&ready_image);
		// ready_image = NULL;
	}
	else if (strcmp(instruction,IMAGE_RECVING)==0)
	{
		// 傳送圖片的一部分
		SDStruct();
	}
	else
		qWarning("undefine instruction");
}

void SctpSocketServer::Scale(float rate)
{
	if (rate <= 0)
	{
		qWarning("the rate is negative");
		rate = 0.25;
	}
	CvSize dst_cvsize;
	dst_cvsize.width = image->width*rate;
	dst_cvsize.height = image->height*rate;

	ready_image = cvCreateImage(dst_cvsize,image->depth,image->nChannels);
	cvResize(image,ready_image,CV_INTER_LINEAR);
}

void SctpSocketServer::SDStruct()
{
	qWarning("widthStep:%d\nheight:%d\nwidth:%d\ntotal:%d",ready_image->widthStep,ready_image->height,ready_image->width,totalsize);

	int ReadyByte;
	int ret;
	char *ptr = (char *) &matrix;

	if (totalsize>=begin+MAX_BUFFER)
	{
		ReadyByte = MAX_BUFFER;
	}
	else
	{
		ReadyByte = totalsize - begin;
	}
	
	if (ReadyByte>0)
	{
		ret = sctp_sendmsg(connfd,ptr+begin,ReadyByte,(struct sockaddr *) NULL,0,0,0,0,0,0);
		if (ret<=0)
		{
			qWarning("sctp_sendmsg error");
			return ;
		}
		else
		{
			//qWarning("send some data");
			begin+=ReadyByte;
		}
	}
	else
	{
		// 表示檔案已傳送結束
		ret = ::sctp_sendmsg(connfd,IMAGE_END,strlen(IMAGE_END),(struct sockaddr *) NULL,0,0,0,0,0,0);
		qWarning("Client imagedata Finish!!");
	}
}
