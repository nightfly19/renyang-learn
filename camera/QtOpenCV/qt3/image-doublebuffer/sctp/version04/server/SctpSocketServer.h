
#ifndef SCTPSOCKETSERVER_H
#define SCTPSOCKETSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#include <qsocketnotifier.h>
#include <qobject.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "common.h"

class SctpSocketServer:public QObject
{
	Q_OBJECT
	public:
		SctpSocketServer(Q_UINT16=MY_PORT_NUM, int backlog=1,QObject *parent=0,const char *name=0);
		void analyze(char *);
		void Scale(float);
		void SDStruct();
	private:
		QSocketNotifier *sctp_newconnection_notification;
		QSocketNotifier *sctp_notification;
		// 表示等待對方連線的socket
		int listenfd;
		// 代表對方的socket fd
		int connfd;
		// 表示server本地端的socket
		struct sockaddr_in serv_addr;

		CvCapture *camera;
		IplImage *image,*ready_image;
		// 用來存放要傳送的圖片資料
		struct image_matrix matrix;
		// 圖片要縮放的大小
		float scale_value;
		// begin記錄圖片要由結構中的哪一個位置開始送
		int begin;
		// 記錄一個圖片一共要傳多少大小
		int totalsize;
	public slots:
		// 接受client的連線
		int SCTPServerAccept(int);
		int recv(int);
};

#endif
