// =========================include header============================
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <string.h>
#include <unistd.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "common.h"
// ====================================================================

//===========================global variable===========================
CvCapture *camera = NULL;
IplImage *image = NULL;
IplImage *ready_image = NULL;
// 用來存放要傳送的圖片資料
struct image_matrix matrix;
float scale_value;
//=====================================================================

// ==========================define function===========================
void analyze(char *,int);
void SDStruct(int);
void Scale(float);
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

	// 輸入要縮放的大小
	printf("please input a scale value (0<scale_value<1):");
	scanf("%f",&scale_value);

	// 建立一個one-to-one style的sctp socket
	listenSock = ::socket(AF_INET,SOCK_STREAM,IPPROTO_SCTP);

	if (listenSock == -1) {
		perror("ERROR opening socket");
		exit(1);
	}

	// 使得可以短時間內再度啟動程式(使用的port是相同的)
	int on=1;
	if (setsockopt(listenSock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))==-1) {
		perror("set SO_RESUEADDR ERROR");
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
	while(1)
	{	client_len = sizeof(cli_addr);
		connSock = ::accept(listenSock,(struct sockaddr *) &cli_addr,&client_len);
		if (connSock == -1) {
			perror("accept error");
			exit(1);
		}
		else
		{
			int flag = 1;
			// 一有資料馬上就送出去
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

		switch (ret)
		{
			case 0:
				printf("the client close the connection\n");
				::close(connSock);
				break;
			case -1:
				printf("the connection has some error\n");
				::close(connSock);
				break;
			default:
				printf("It is impossible!\n");
		}
	}
	::close(listenSock);
	return 0;
}

//===============================function implement=====================
void analyze(char *instruction,int connfd)
{
	int ret;
	// 若分析到的字串是client要求開始傳送資料
	if (strcmp(instruction,IMAGE_START)==0)
	{
		if (camera == NULL) {
			// 設定要使用的webcam設備
			camera = cvCreateCameraCapture(0);
			// 回傳是NULL表示沒有webcam的設備
			if (camera == NULL)
			{
				// 通知client取得frame過程錯誤
				ret = sctp_sendmsg(connfd,IMAGE_ERROR,strlen(IMAGE_ERROR),(struct sockaddr *) NULL,0,0,0,0,0,0);
#ifdef REN_DEBUGgg
				if (ret <= 0)
					perror("send IMAGE_ERROR error");
				else
					printf("send IMAGE_ERROR");
#endif
			}
		}
		// 嘗試取得第一個frame
		if ((image=cvQueryFrame(camera))==NULL)
		{
			// 通知client取得frame過程錯誤
			ret = sctp_sendmsg(connfd,IMAGE_ERROR,strlen(IMAGE_ERROR),(struct sockaddr *) NULL,0,0,0,0,0,0);
#ifdef REN_DEBUG
			if (ret <= 0)
				perror("send IMAGE_ERROR error");
			else
				printf("send IMAGE_ERROR");
#endif
		}
		else
		{
			// 通知client可以正常取得frame
			ret = sctp_sendmsg(connfd,IMAGE_OK,strlen(IMAGE_OK),(struct sockaddr *) NULL,0,0,0,0,0,0);
#ifdef REN_DEBUG
			if (ret <= 0)
				perror("send IMAGE_OK error");
			else
				printf("send IAMGE_OK");
#endif
		}
	}
	else if (strcmp(instruction,IMAGE_SPACE_PREPARE_OK)==0)
	{
		// client端表示它的buffer空間準備好啦
		printf("the image buffer prepare ok\n");
		// 縮放Image
		Scale(scale_value);
		// 填入要傳送的資料
		matrix.height = ready_image->height;
		matrix.width = ready_image->width;
		memcpy(matrix.data,ready_image->imageData,3*matrix.height*matrix.width);
		// 傳送結構啦
		SDStruct(connfd);
		// 把資料刪除
		cvReleaseImage(&ready_image);
		ready_image = NULL;
	}
	else
		printf("undefine instruction\n");
}

void SDStruct(int connfd)
{
	// 計算總共要傳送的檔案大小, 加8是表示兩個額外的int
	int totalsize = 3*(ready_image->height)*(ready_image->width)+8;
	printf("widthStep:%d\nheight:%d\nwidth:%d\ntotal:%d\n",ready_image->widthStep,ready_image->height,ready_image->width,totalsize);
	// begin記錄目前送到哪裡啦
	int begin=0;
	// 把一個檔案分成許多個檔案, 計算目前這一個要傳送多大的資料
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
#ifdef REN_DEBUG
			if (ret<=0)
			{
				perror("sctp_sendmsg error\n");
				return ;
			}
			else
			{
				// printf("send some data\n");
				printf("send imagedata");
			}
#endif
			bzero(recvbuff,MAX_BUFFER);
			// 等待對方確實收到資料
			ret = sctp_recvmsg(connfd,recvbuff,MAX_BUFFER,(struct sockaddr *) NULL,(socklen_t *) NULL,NULL,NULL);
			if (ret <=0)
			{
				perror("sctp_recvmsg error\n");
				return ;
			}
			else
			{
#ifdef REN_DEBUG
				printf("recv image data");
#endif
				begin+=ReadByte;
			}
		}
		else
		{
			break;
		}
	}while(true);
	// 表示檔案已傳送結束
	ret = sctp_sendmsg(connfd,IMAGE_END,strlen(IMAGE_END),(struct sockaddr *) NULL,0,0,0,0,0,0);
#ifdef REN_DEBUG
	if (ret <= 0)
		perror("send IMAGE_END error");
	else
		printf("send IMAGE_END");
#endif
	printf("\nClient imagedata Finish!!\n");
}

void Scale(float rate)
{
	if (rate <= 0)
	{
		printf("the rate is negative\n");
		exit(1);
	}
	CvSize dst_cvsize;
	dst_cvsize.width = image->width*rate;
	dst_cvsize.height = image->height*rate;

	ready_image = cvCreateImage(dst_cvsize,image->depth,image->nChannels);
	cvResize(image,ready_image,CV_INTER_LINEAR);
}
//======================================================================
