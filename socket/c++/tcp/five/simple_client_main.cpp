#include "ClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include "imagedata.h"
using namespace std;

void RecvFile(ClientSocket *client_socket,char *filename);
void RecvStruct(ClientSocket *client_socket,char *pixel);

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
		memset(&image,0,sizeof(struct imagedata));

		try
		{
			*client_socket << "Test message.";
			//client_socket >> buf;
			//::RecvFile(client_socket,"realalt180.exe");
			::RecvStruct(client_socket,(char *)&image);
			printf("what\n");
		}
		catch(SocketException&){}
		// printf("We received : %s\n",buf);
		cout << image.pixel[0][0] << endl;
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

void RecvStruct(ClientSocket *client_socket,char *pixel)
{
	char filebuffer[MAXRECV];
	memset(filebuffer,0,MAXRECV);
	int Readbyte=0;
	int begin=0;
	Readbyte=*client_socket>>filebuffer;
	while(strncmp(filebuffer,"#over#",strlen("#over#"))!=0)
	{
		strncpy(pixel+begin,filebuffer,Readbyte);
		begin+=Readbyte;
		Readbyte=*client_socket>>filebuffer;
		printf("y");
	}
}
