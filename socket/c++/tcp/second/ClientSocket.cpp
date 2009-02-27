// Implementation of the ClientSocket class

#include "ClientSocket.h"
#include "SocketException.h"


// 建立一個連線到host且port號為port的connfd
ClientSocket::ClientSocket ( std::string host, int port )
{
  // 建立socketfd
  if ( ! Socket::create() )
    {
      throw SocketException ( "Could not create client socket." );
    }

  // 連線到server
  if ( ! Socket::connect ( host, port ) )
    {
      throw SocketException ( "Could not bind to port." );
    }

}


const ClientSocket& ClientSocket::operator << ( const std::string& s ) const
{
  if ( ! Socket::send ( s ) )
    {
      throw SocketException ( "Could not write to socket." );
    }

  return *this;

}


const ClientSocket& ClientSocket::operator >> ( std::string& s ) const
{
  if ( ! Socket::recv ( s ) )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return *this;
}
