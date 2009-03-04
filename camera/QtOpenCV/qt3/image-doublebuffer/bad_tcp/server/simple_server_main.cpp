#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
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

	while(true)
	{
		try
		{
			struct imagedata image;
			// 把第一個資料填入
			memset(&image,1,sizeof(struct imagedata));	// 不可以填0,否則會出現錯誤,可能是所有成員都必需要有資料
			image.r_pixel[0][0]='P';	// 先填一個數值進去,到client端再讀取出來,看是否有錯誤
			char data[MAXRECV];
			/*
			 * 先把image的資料放到file裡面
			FILE *fp;
			fp = fopen("original","wb");
			fwrite(&image,1,sizeof(struct imagedata),fp);
			fclose(fp);
			 */
			while(true)
			{
				memset(data,0,MAXRECV);
				//*server >> data;
				//::SDFile(server,"realalt180.exe");
				::SDStruct(server,(char*)&image);	// 送資料
				printf("%s\n",data);
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
