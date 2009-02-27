#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
using namespace std;

int main(int argc,char **argv) {
	cout << "running...." << endl;

	try
	{
		ServerSocket server(30000);
	}
	catch ( SocketException& e )
	{
		cout << "Exception was caught:" << e.description() << "\nExiting.\n";
	}
	return 0;
}
