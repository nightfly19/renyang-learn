#include "ClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include "imagedata.h"
#include "transmit.h"
using namespace std;

int main(int argc,char **argv)
{
	char ipaddr[15];
	memset(ipaddr,0,sizeof(ipaddr));
	if (argc==2)
	{
		strcpy(ipaddr,argv[1]);
	}
	else
	{
		strcpy(ipaddr,"127.0.0.1");
	}
	try
	{
		ClientSocket *client_socket;
		client_socket = new ClientSocket(ipaddr,30000);

		char buf[MAXRECV];
		memset(buf,0,MAXRECV);
		struct imagedata image;
		memset(&image,1,sizeof(struct imagedata));

		try
		{
			//*client_socket << "Test message.";
			//client_socket >> buf;
			//::RecvFile(client_socket,"realalt180.exe");
			::RecvStruct(client_socket,(char *)&image);
			printf("what\n");
		}
		catch(SocketException&){}
		// printf("We received : %s\n",buf);
		cout << image.r_pixel[0][0] << endl;
		// 把資料寫到檔案after - start
		FILE *fp;
		fp = fopen("after","wb");
		fwrite(&image,1,sizeof(struct imagedata),fp);
		fclose(fp);
		// 把資料寫到檔案after - end
	}
	catch (SocketException &e)
	{
		std::cout << "Exception was caught:" << e.description() << "\n";
	}
	return 0;
}
