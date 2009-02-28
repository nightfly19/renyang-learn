#include "ClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
using namespace std;

int main(int argc,char **argv)
{
	try
	{
		ClientSocket client_socket("localhost",30000);

		char buf[MAXRECV];
		memset(buf,0,MAXRECV);

		try
		{
			client_socket << "Test message.";
			client_socket >> buf;
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
