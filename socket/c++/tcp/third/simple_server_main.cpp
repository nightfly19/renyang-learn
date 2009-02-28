#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
using namespace std;

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
				*server << data;
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
