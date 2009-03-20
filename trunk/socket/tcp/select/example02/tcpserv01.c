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

//======================main function===========================
int main(int argc,char **argv)
{
	int i,maxi,maxfd,listenfd,connfd,sockfd;
	int nready;
	int client[FD_SETSIZE];	//  記錄所有用來記錄client的file descriptor的陣列
	ssize_t n;
	fd_set rset,allset;
	char buf[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr,servaddr;
	int ret_value=0;

	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if (listenfd == -1) {
		perror("socket error");
		exit(-1);
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	ret_value = bind(listenfd,(struct sockaddr *) &servaddr,sizeof(servaddr));

	ret_value = listen(listenfd,LISTENQ);

	maxfd = listenfd;	// initialize
	maxi = -1;		// 記錄client array的最大的數
	for (i=0;i<FD_SETSIZE;i++) {
		client[i]=-1;
	}
	FD_ZERO(&allset);
	FD_SET(listenfd,&allset);

	for (;;) {
		rset = allset;	// structure assignment
		nready = select(maxfd+1,&rset,NULL,NULL,NULL);

		if (FD_ISSET(listenfd,&rset)) {	// new client connection
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd,(struct sockaddr *) &cliaddr,&clilen);

			for (i=0;i<FD_SETSIZE;i++) {
				if (client[i]<0) {
					client[i]=connfd; // save descriptor
					break;
				}
			}
			
			if (i == FD_SETSIZE) {
				printf("too many clients\n");
				exit(-1);
			}

			FD_SET(connfd,&allset);	// add new descriptor to set

			if (connfd > maxfd) {
				maxfd = connfd;	// for select
			}

			if (i > maxi) {
				maxi=i;		// max index in client[] array
			}

			if (--nready <=0) {
				continue;	// no more readable descriptors
			}
		}

		for (i=0;i<=maxi;i++) {
			if ((sockfd = client[i] ) < 0)
				continue;
			if (FD_ISSET(sockfd,&rset)) {
				memset(buf,0,sizeof(buf));
				if (( n = read(sockfd,buf,MAXLINE))==0) {	// 表示client端單方面結束connection
					close(sockfd);
					FD_CLR(sockfd,&allset);
					client[i]=-1;
				}
				else
				{
					ret_value = write(sockfd,buf,n);
					if (ret_value < 0) {
						perror("write error");
						exit(-1);
					}
				}
				if (--nready<=0)
				{
					break;
				}
			}
		}
	}
}
