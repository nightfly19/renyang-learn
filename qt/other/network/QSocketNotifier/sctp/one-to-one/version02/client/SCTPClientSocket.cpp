#include <arpa/inet.h>

#include "SCTPClientSocket.h"

SCTPClientSocket::SCTPClientSocket(QObject *parent,const char *name):QObject(parent,name)
{
	int ret;
	struct sctp_initmsg initmsg;
	connSock = -1;
	sctp_notification = NULL;

	connSock = ::socket(AF_INET, SOCK_STREAM,IPPROTO_SCTP);
	if (connSock == -1) {
		perror("socket error");
		exit(1);
	}

	memset(&initmsg,0,sizeof(initmsg));

	initmsg.sinit_num_ostreams = 5;
	initmsg.sinit_max_instreams = 5;
	initmsg.sinit_max_attempts = 4;

	ret = setsockopt(connSock,IPPROTO_SCTP,SCTP_INITMSG,&initmsg,sizeof(initmsg));
	if (ret < 0) {
		perror("setsockopt error");
		exit(1);
	}
}

void SCTPClientSocket::connectToHost(const QString &host, Q_UINT16 port)
{
	struct sockaddr_in servaddr;
	int ret;

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(host.latin1());

	ret = ::connect(connSock,(struct sockaddr *) &servaddr,sizeof(servaddr));
	if (ret == -1) {
		perror("connect error");
		exit(1);
	}
	qWarning("connectToHost success!");
}

int SCTPClientSocket::SCTPSendmsg(const void *msg,size_t len,struct sockaddr *to,socklen_t tolen, uint32_t ppid,uint32_t flags,uint16_t stream_no, uint32_t timetolive, uint32_t context)
{
	int ret;
	ret = sctp_sendmsg(connSock, msg,len,to,tolen,ppid,flags,stream_no,timetolive,context);
	if (ret < 0) {
		perror("sctp_sendmsg error");
		exit(-1);
	}
	return ret;
}
