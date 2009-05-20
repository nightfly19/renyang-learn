// =========================include header============================
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <string.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "common.h"
// ====================================================================

//===========================global variable===========================
CvCapture *camera = NULL;
IplImage *image = NULL;
struct image_matrix matrix;
//=====================================================================

// ==========================define function===========================
void analyze(char *,int);
void SDStruct(int);
// ====================================================================

int main(int argc,char **argv)
{
	int listenSock, connSock, flags;
	int ret;
	struct sockaddr_in serv_addr,cli_addr;
	socklen_t server_len,client_len;
	struct sctp_sndrcvinfo sndrcvinfo;
	char buffer[MAX_BUFFER];
	client_len = sizeof(cli_addr);

	listenSock = ::socket(AF_INET,SOCK_STREAM,IPPROTO_SCTP);

	if (listenSock == -1) {
		perror("ERROR opening socket");
		exit(1);
	}

	printf("connSock : %d\n",listenSock);

	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(MY_PORT_NUM);

	// bind - bind a name to a socket
	if ((::bind(listenSock,(struct sockaddr *) &serv_addr, sizeof(serv_addr)))==-1) {
		perror("listen error");
		exit(1);
	}

	if (::listen(listenSock,5)==-1) {
		perror("listen error");
		exit(1);
	}

	printf("Accepting connections ...\n");
	client_len = sizeof(cli_addr);
	connSock = ::accept(listenSock,(struct sockaddr *) &cli_addr,&client_len);
	if (connSock == -1) {
		perror("accept error");
		exit(1);
	}
	else
	{
		int flag = 1;
		ret = ::setsockopt(connSock,IPPROTO_SCTP,SCTP_NODELAY,&flag,sizeof(flag));
		if (ret == -1) {
			perror("setsockopt SCTP_NODELAY error");
		}
		printf("accept the client connect...\n");
	}
	// 等待client傳送指令過來
	do {
		bzero(buffer,sizeof(buffer));
		ret = sctp_recvmsg(connSock,buffer,sizeof(buffer),(struct sockaddr *) NULL,(socklen_t *) NULL,&sndrcvinfo,&flags);
		analyze(buffer,connSock);
	}while(ret != -1 && ret != 0);

	return 0;
}

//===============================function implement=====================
void analyze(char *instruction,int connfd)
{
	int ret;
	if (strcmp(instruction,IMAGE_START)==0)
	{
		if (camera == NULL) {
			camera = cvCreateCameraCapture(0);
			if (camera == NULL)
				ret = sctp_sendmsg(connfd,IMAGE_ERROR,strlen(IMAGE_ERROR),(struct sockaddr *) NULL,0,0,0,0,0,0);
		}
		if ((image=cvQueryFrame(camera))==NULL)
			ret = sctp_sendmsg(connfd,IMAGE_ERROR,strlen(IMAGE_ERROR),(struct sockaddr *) NULL,0,0,0,0,0,0);
		else
			ret = sctp_sendmsg(connfd,IMAGE_OK,strlen(IMAGE_OK),(struct sockaddr *) NULL,0,0,0,0,0,0);
	}
	else if (strcmp(instruction,IMAGE_SPACE_PREPARE_OK)==0)
	{
		printf("the image buffer prepare ok\n");
		matrix.height = image->height;
		matrix.width = image->width;
		memcpy(matrix.data,image->imageData,3*matrix.height*matrix.width);
		SDStruct(connfd);
	}
	else
		printf("undefine instruction\n");
}

void SDStruct(int connfd)
{
	int totalsize = 3*(image->height)*(image->width)+8;
	printf("widthStep:%d\nheight:%d\nwidth:%d\ntotal:%d\n",image->widthStep,image->height,image->width,totalsize);
	int begin=0;
	int ReadByte=0;
	int ret;
	char recvbuff[MAX_BUFFER];
	char *ptr = (char *)&matrix;
	do
	{
		if (totalsize>=begin+MAX_BUFFER)
		{
			ReadByte = MAX_BUFFER;
		}
		else
		{
			ReadByte = totalsize-begin;
		}
		if (ReadByte>0)
		{
			ret = sctp_sendmsg(connfd,ptr+begin,ReadByte,(struct sockaddr *) NULL,0,0,0,0,0,0);
			if (ret<=0)
			{
				printf("sctp_sendmsg error\n");
				return ;
			}
			else
			{
				// printf("send some data\n");
			}
			bzero(recvbuff,MAX_BUFFER);
			ret = sctp_recvmsg(connfd,recvbuff,MAX_BUFFER,(struct sockaddr *) NULL,(socklen_t *) NULL,NULL,NULL);
			if (ret <=0)
			{
				printf("sctp_recvmsg error\n");
				return ;
			}
			else
			{
				// printf("got some data\n");
				begin+=ReadByte;
			}
		}
		else
		{
			break;
		}
	}while(true);
	ret = sctp_sendmsg(connfd,IMAGE_END,strlen(IMAGE_END),(struct sockaddr *) NULL,0,0,0,0,0,0);
	printf("\nClient imagedata Finish!!\n");
}
//======================================================================
