#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
using namespace std;

void SDFile(ServerSocket *server,char *filename);

int main(int argc,char **argv) {
	cout << "running...." << endl;
	ServerSocket *server;

	try
	{
		server = new ServerSocket(30000);
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
			char data[MAXRECV];
			while(true)
			{
				memset(data,0,MAXRECV);
				*server >> data;
				::SDFile(server,"realalt180.exe");
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
//==========================function implement====================
void SDFile(ServerSocket *server,char *filename)
{
	char sdbuffer[MAXRECV];
	char end[7]="#over#";
	int ReadByte=0;
	FILE *sdfile = fopen(filename,"rb");
	if (sdfile==NULL)
	{
		printf("open %s file error!!\n",filename);
		*server << end;
		return ;
	}
	printf("Client: sending file <%s>",filename);

	while((ReadByte=fread(sdbuffer,sizeof(char),MAXRECV,sdfile))>0)
	{
		printf("fread:%d\n",ReadByte);
		//printf(".");
		server->Sendbyte(sdbuffer,ReadByte);
		memset(sdbuffer,0,MAXRECV);
	}
	fclose(sdfile);
	// 若這邊不sleep一下的話,上面的最後一個片段會與下面的結束片斷一起傳送,就會造成傳送過去的資料判斷錯誤
	// 透過non-blocking就可以避免使用sleep啦
	// sleep(1);
	*server<<end;
	printf("\nClient:<%s> Finish!!\n",filename);
}
