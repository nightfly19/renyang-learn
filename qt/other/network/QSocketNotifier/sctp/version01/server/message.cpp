#include "message.h"

message::message()
{
	sn = NULL;
	sock_fd = socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
	if (sock_fd == -1)
	{
		qWarning("socket error");
		exit(-1);
	}
	sn = new QSocketNotifier(sock_fd,QSocketNotifier::Read,this);
	connect(sn,SIGNAL(activated(int)),this,SLOT(printsys()));

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(1793);

	ret_value = ::bind(sock_fd,(struct sockaddr *) &servaddr,sizeof(servaddr));
	if (ret_value == -1)
	{
		qWarning("bind error");
		exit(-1);
	}
	ret_value = listen(sock_fd,5);
	if (ret_value == -1)
	{
		qWarning("listen error");
		exit(-1);
	}
	qWarning("start waiting...");
}

void message::printsys()
{
	len = sizeof(struct sockaddr_in);
	rd_sz = sctp_recvmsg(sock_fd,readbuf,sizeof(readbuf),(struct sockaddr *) &cliaddr,&len,&sri,&msg_flags);
	ret_value = sctp_sendmsg(sock_fd,readbuf,rd_sz,(struct sockaddr *) &cliaddr,len,sri.sinfo_ppid,sri.sinfo_flags,sri.sinfo_stream,0,0);
	if (ret_value == -1)
	{
		qWarning("sctp_sendmsg error");
		exit(-1);
	}
}
