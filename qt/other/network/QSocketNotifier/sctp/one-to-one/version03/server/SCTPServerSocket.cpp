#include "SCTPServerSocket.h"

SCTPServerSocket::SCTPServerSocket()
{
	sctp_newconnection_notification = NULL;
}

SCTPServerSocket::SCTPServerSocket(Q_UINT16 port,int backlog, QObject *parent,const char *name):QObject(parent,name)
{
	int ret;

	sctp_newconnection_notification = NULL;
	sctp_notification = NULL;
	listenfd = ::socket(AF_INET,SOCK_STREAM,IPPROTO_SCTP);
	if (listenfd == -1) {
		perror("ERROR opening socket");
		exit(1);
	}
	
	::bzero(&serv_addr,sizeof(struct sockaddr_in));

	initmsg.sinit_num_ostreams = 5;
	initmsg.sinit_max_instreams = 5;
	initmsg.sinit_max_attempts = 4;
	ret = ::setsockopt(listenfd,IPPROTO_SCTP,SCTP_INITMSG,&initmsg,sizeof(initmsg));
	if (ret < 0) {
		perror("ERROR setsockopt");
		exit(1);
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if ((::bind(listenfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)))==-1) {
		perror("Bind error");
		exit(1);
	}

	// listen - listen for socket connections and limit the queue of incoming connections
	if (::listen(listenfd,backlog)==-1) {
		perror("listen error");
		exit(1);
	}

	sctp_newconnection_notification = new QSocketNotifier(listenfd,QSocketNotifier::Read,this,"sctp_newconnection_notification");
	connect(sctp_newconnection_notification,SIGNAL(activated(int)),this,SLOT(SCTPServerAccept(int)));
	qWarning("the serversock is %d",listenfd);
	qWarning("Accepting connections ...");
}

int SCTPServerSocket::SCTPServerAccept(int s)
{
	int connfd;
	struct sockaddr_in cli_addr;
	socklen_t client_len;
	client_len = sizeof(cli_addr);
	connfd = ::accept(s,(struct sockaddr *) &cli_addr,&client_len);
	if (connfd == -1) {
		perror("accept error");
		exit(-1);
	}
	emit newConnection(connfd);
	sctp_notification = new QSocketNotifier(connfd,QSocketNotifier::Read,this,"sctp_notification");
	connect(sctp_notification,SIGNAL(activated(int)),this,SLOT(recvMsg(int)));
	qWarning("accept success!!");
	qWarning("the connfd is %d",connfd);
	return connfd;
}

int SCTPServerSocket::recvMsg(int s)
{
	int ret;
	char recvbuffer[1024];
	bzero(recvbuffer,1024);
	ret = sctp_recvmsg(s,recvbuffer,1024,(struct sockaddr *) NULL,0,(struct sctp_sndrcvinfo *) NULL, (int *) NULL);
	if (ret == -1) {
		perror("sctp_recvmsg error");
		exit(-1);
	}
	else if (ret == 0) {
		delete sctp_notification;
		sctp_notification = NULL;
		close(s);
		qWarning("client close");
		return ret;
	}
	qWarning("sctp_recvmsg success!!");
	qWarning("%s",recvbuffer);
	return ret;
}
