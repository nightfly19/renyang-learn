#include "ClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
using namespace std;

void RecvFile(ClientSocket *client_socket,char *filename);

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
		strcpy(ipaddr,"localhost");
	}
	try
	{
		ClientSocket *client_socket;
		client_socket = new ClientSocket(ipaddr,30000);

		char buf[MAXRECV];
		memset(buf,0,MAXRECV);

		try
		{
			*client_socket << "Test message.";
			//client_socket >> buf;
			::RecvFile(client_socket,"realalt180.exe");
			printf("what\n");
		}
		catch(SocketException&){}
		printf("We received : %s\n",buf);
	}
	catch (SocketException &e)
	{
		std::cout << "Exception was caught:" << e.description() << "\n";
	}
	return 0;
}
//======================function implement==========================
void RecvFile(ClientSocket *client_socket,char *filename)
{
	FILE *fp = fopen(filename,"wb");
	char filebuffer[MAXRECV];
	memset(filebuffer,0,MAXRECV);
	int Readbyte=0;
	Readbyte=*client_socket>>filebuffer;
	while(strncmp(filebuffer,"#over#",strlen("#over#"))!=0)
	{
		fwrite(filebuffer,sizeof(char),Readbyte,fp);
		Readbyte=*client_socket>>filebuffer;
		printf("x");
	}
	fclose(fp);
}
