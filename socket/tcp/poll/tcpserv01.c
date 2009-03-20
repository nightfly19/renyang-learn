//======================include file============================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <limits.h>	// for OPEN_MAX
//==============================================================

//======================define variable=========================
#include "global.h"
//==============================================================

//======================main function===========================
int main(int argc,char **argv)
{
	int i,maxi,listenfd,connfd,sockfd;
	int nready;
	ssize_t n;
	char buf[MAXLINE];
	socklen_t clilen;
	struct pollfd client[OPEN_MAX];
	struct sockaddr_in cliaddr,servaddr;
	int ret_value =0;

	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if (listenfd == -1) {
		perror("socket error");
		exit(-1);
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	ret_value = bind(listenfd,(struct sockaddr*) &servaddr,sizeof(servaddr));
	if (ret_value == -1) {
		perror("bind error");
		exit(-1);
	}

	ret_value = listen(listenfd,LISTENQ);
	if (ret_value == -1) {
		perror("listen error");
		exit(-1);
	}

	client[0].fd = listenfd;	// 設定第一個要注意的file descriptor就是listen file descriptor
	client[0].events = POLLRDNORM;	// 設定要注意的事件,normal data can be read
	for (i=1;i<OPEN_MAX;i++) {
		client[i].fd = -1;	// -1 indicates available entry
	}
	maxi=0;	// max index into client[] array
	for (;;) {
		nready = poll(client,maxi+1,INFTIM);	// 一直等待,直到有event發生
		if (client[0].revents & POLLRDNORM) {	// 是否有指定的事件發生,POLLRDNORM:normal data can be read
			clilen = sizeof(cliaddr);
			printf("before accept\n");
			connfd = accept(listenfd,(struct sockaddr*) &cliaddr,&clilen);
			printf("after accept\n");
			if (connfd == -1) {
				perror("accept error");
				exit(-1);
			}

			for (i=1;i<OPEN_MAX;i++) {
				if (client[i].fd < 0) {	// 由頭開始找,找到第一個沒有被使用的陣列位置
					client[i].fd = connfd;	// save descriptor
					break;
				}
			}
			if (i == OPEN_MAX) {
				printf("too many clients\n");
				exit(-1);	// 在思考server是否一定要跳出來??
			}
			client[i].events = POLLRDNORM;
			if (i > maxi) {
				maxi = i;	// max index in client[] array
			}

			if (--nready <= 0) {
				continue;	// no more readable descriptors
			}
		}

		for (i=1;i<=maxi;i++) {
			if ((sockfd = client[i].fd)<0) {
				continue;
			}
			if (client[i].revents & (POLLRDNORM | POLLERR)) {
				if ((n=read(sockfd,buf,MAXLINE))<0) {
					if (errno == ECONNRESET) {
						// connection reset by client
						close(sockfd);
						client[i].fd = -1;
					}
					else {
						printf("read error\n");
						exit(-1);
					}
				}
				else if (n == 0) {
					close(sockfd);
					client[i].fd = -1;
				}
				else
				{
					write(sockfd,buf,n);
				}

				if (--nready<=0) {
					break;
				}
			}
		}
	}
}
