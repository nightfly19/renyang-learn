#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "imagedata.h"
#include "transmit.h"
using namespace std;

int main(int argc,char **argv) {
	cout << "running...." << endl;
	ServerSocket *server;

	try
	{
		server = new ServerSocket(30000);	// 等待client連線
	}
	catch ( SocketException& e )
	{
		cout << "Exception was caught:" << e.description() << "\nExiting.\n";
		return -1;
	}

	CvCapture *camera = cvCreateCameraCapture(0);
	assert(camera);
	while(true)
	{
		try
		{
			// 把第一個資料填入
			char buffer[MAXRECV];
			/*
			FILE *fp;
			fp = fopen("before","wb");
			fwrite(&data,1,sizeof(struct imagedata),fp);
			fclose(fp);
			printf("write ok\n");
			*/
			while(true)
			{
			IplImage * image=cvQueryFrame(camera);
			assert(image);
			struct imagedata data;
			Image2imagedata(image,data);
				memset(buffer,0,MAXRECV);
				//*server >> data;
				//::SDFile(server,"realalt180.exe");
				::SDStruct(server,(char*)&data);	// 送資料
			}
		}
		catch (SocketException &)
		{
			server->close_connfd();
			server->accept();
		}
	}
	return 0;
}
