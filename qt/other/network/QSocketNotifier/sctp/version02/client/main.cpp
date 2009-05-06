#include <qapplication.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

//=======================define function==================
void sctpstr_cli(FILE *fp,int sock_fd,struct sockaddr *to,socklen_t tolen);
//========================================================

int main(int argc,char **argv)
{
	int sock_fd;
	struct sockaddr_in servaddr;
	struct sctp_event_subscribe evnts;
	int ret_value=0;
	
	if (argc < 2)
	{
		qWarning("Missing host argument - use '%s host'\n",argv[0]);
		exit(-1);
	}

	sock_fd = socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);

	// set non-blocking
	if (fcntl(sock_fd,F_SETFL,O_NONBLOCK)<0)
	{
		qWarning("PID_UP::fcntl:O_NONBLOCK error");
		exit(-1);
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(1793);
	ret_value = inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
	if (ret_value != 1)
	{
		qWarning("translate server ip address error!!");
		exit(-1);
	}
	memset(&evnts,0,sizeof(evnts));
	evnts.sctp_data_io_event = 1;
	ret_value = setsockopt(sock_fd,IPPROTO_SCTP,SCTP_EVENTS,&evnts,sizeof(evnts));
	if (ret_value == -1)
	{
		qWarning("setsockopt error");
		exit(-1);
	}
	sctpstr_cli(stdin,sock_fd,(struct sockaddr *) &servaddr,sizeof(servaddr));
	close(sock_fd);
	return 0;
}

//======================function implement====================
void sctpstr_cli(FILE *fp,int sock_fd,struct sockaddr *to,socklen_t tolen)
{
	struct sockaddr_in peeraddr;
	struct sctp_sndrcvinfo sri;
	char sendline[256],recvline[256];
	socklen_t len;
	int out_sz,rd_sz;
	int msg_flags;
	int ret_value=0;

	bzero(&sri,sizeof(sri));
	while (fgets(sendline,256,fp) != NULL)
	{
		sri.sinfo_stream = 0;
		out_sz = strlen(sendline);
		ret_value = sctp_sendmsg(sock_fd,sendline,out_sz,to,tolen,0,0,sri.sinfo_stream,0,0);
		if (ret_value == -1)
		{
			qWarning("sctp_sendmsg error\n");
			exit(-1);
		}
		len = sizeof(peeraddr);
		rd_sz = sctp_recvmsg(sock_fd,recvline,sizeof(recvline),(struct sockaddr *) &peeraddr,&len,&sri,&msg_flags);
		printf("From str:%d seq:%d (assoc:0x%x):",sri.sinfo_stream,sri.sinfo_ssn,(u_int) sri.sinfo_assoc_id);
		printf("%.*s",rd_sz,recvline);
	}
}
