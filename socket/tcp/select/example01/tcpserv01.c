//======================include file============================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//==============================================================

//======================define variable=========================
#include "global.h"
//==============================================================

//======================define function=========================
void str_echo(int sockfd);
//==============================================================

//======================main function===========================
int main(int argc,char **argv)
{
	int listenfd,connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr,servaddr;
	int ret_value=0;

	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if (listenfd == -1) {
		perror("socket error");
		exit(-1);
	}
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	ret_value = bind(listenfd,(struct sockaddr *) &servaddr,sizeof(servaddr));
	if (ret_value == -1) {
		perror("bind error");
		exit(-1);
	}

	ret_value = listen(listenfd,LISTENQ);

	for (;;) {
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd,(struct sockaddr *) &cliaddr,&clilen);

		if ((childpid == fork()) == 0) {	// 等於0表示是child process
			close(listenfd);		// close listening socket
			str_echo(connfd);		// process the request
			exit(0);
		}
		close(connfd);				// parent closes connected socket
	}
	return 0;
}
//==============================================================

//======================function implement======================
void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];
	int ret_value=0;
again:
	while((n = read(sockfd,buf,MAXLINE))>0) {
		ret_value = write(sockfd,buf,n);
		printf("%s",buf);
		memset(buf,0,sizeof(buf));
	}
	if (n<0 && errno==EINTR) {
		printf("n<0 && errno==EINTER\n");
		goto again;
	}
	else
		printf("str_echo: read_error");
}
