// Implementation of the ServerSocket class

#include "ServerSocket.h"
#include "SocketException.h"


ServerSocket::ServerSocket ( int port )
{
  // 因為繼承Socket,所以可以直接使用Socket類別的函數
  if ( ! Socket::create() )
    {
      throw SocketException ( "Could not create server socket." );
    }

  if ( ! Socket::bind ( port ) )
    {
      throw SocketException ( "Could not bind to port." );
    }

  if ( ! Socket::listen() )
    {
      throw SocketException ( "Could not listen to socket." );
    }

}

ServerSocket::~ServerSocket()
{
	// do nothing
}


// 把資料傳送到client端
const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
{
  if ( ! Socket::send ( s ) )
    {
      throw SocketException ( "Could not write to socket." );
    }

  return *this;

}


// 由client端接收資料
const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
{
  if ( Socket::recv ( s )<=0 )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return *this;
}

void ServerSocket::accept ( ServerSocket& sock )
{
  // 呼叫c類別內建的accept函數
  if ( ! Socket::accept ( sock ) )
    {
      // 建立一個例外錯誤物件丟出去
      throw SocketException ( "Could not accept socket." );
    }
}
