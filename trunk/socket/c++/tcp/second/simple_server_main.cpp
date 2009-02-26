#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>

// ServerSocket是用來儲存serverfd
// 而ServerSocket new_sock 是用來儲存connfd
int main ( int argc, int argv[] )
{
  std::cout << "running....\n";

  try
    {
      // Create the socket, 使用port:30000
      ServerSocket server ( 30000 );

      while ( true )
	{

	  ServerSocket new_sock;
	  // 產生一個新的server socket,用來存connectfd的資料
	  server.accept ( new_sock );

	  try
	    {
	      while ( true )
		{
		  // 建立一個資料準備把資料放入data中
		  std::string data;
		  // 把由connfd接收到的資料放到data物件中
		  new_sock >> data;
		  // 把接收到的資料回傳到connfd中
		  new_sock << data;
		  std::cout << data << std::endl;
		}
	    }
	  catch ( SocketException& ) {}

	}
    }
  catch ( SocketException& e )
    {
      std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }

  return 0;
}
